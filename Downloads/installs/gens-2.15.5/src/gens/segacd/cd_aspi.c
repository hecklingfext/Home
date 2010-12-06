#include <string.h>
#include <stdio.h>
#include "port.h"
#include "g_main.h"
#include "rom.h"
#include "star_68k.h"
#include "lc89510.h"
#include "cd_aspi.h"
#include "mem_s68k.h"

#define HIBYTE(x) x >> 8
#define LOBYTE(x) x & 0xff
void Sleep (int i);

//By Ubi: Using cdrom linux.
//Not needed use all ASPI functions, we can delete it...
#include <fcntl.h>
#include <sys/ioctl.h>
#include <linux/cdrom.h>

//char CDROM_DEV[64] = "/dev/hdc";
//cdrd: to read from ioctl.
union cdrd
{
  struct cdrom_msf0 msf;
  char buf[2048 + 512];
};

int FD_CDROM = 0;
int LBA_POS = 0;
int MAX_LBA = 0;

static inline int
MSF2LBA (struct cdrom_msf0 msf)
{
  return (((msf.minute * CD_SECS) + msf.second) * CD_FRAMES + msf.frame) -
    CD_MSF_OFFSET;
}
static inline struct cdrom_msf0
LBA2MSF (int lba)
{
  struct cdrom_msf0 msf;
  lba += CD_MSF_OFFSET;
  msf.frame = lba % CD_FRAMES;
  lba /= CD_FRAMES;
  msf.second = lba % CD_SECS;
  lba /= CD_SECS;
  msf.minute = lba;
  return msf;
}

//End By Ubi.

DWORD (*Get_ASPI_Info) (void);
DWORD (*Get_ASPI_Version) (void);
DWORD (*Send_ASPI_Command) (LPSRB);
int ASPI_Command_Running;
int CDROM_SPEED;
int Num_CD_Drive;
int CUR_DEV; // a che serve?
int DEV_PAR[8][3];
unsigned int Current_LBA;
BYTE Buf_Stat[256];
SRB_ExecSCSICmd se;
TOC toc;

// for CDC functions

int Read_Complete = 1;
unsigned char Buf_Read[2366 * 64];
SRB_ExecSCSICmd sread;


int
ASPI_Init (void)
{
  ASPI_Command_Running = 0;
  Num_CD_Drive = 0;
  Current_LBA = 0;

  //By Ubi Start:
  FD_CDROM = open (CDROM_DEV, O_RDONLY | O_NONBLOCK);
  if (FD_CDROM >= 0)
    {
      LINUXCD_Select_Speed();
      Num_CD_Drive = 1;
      return 0; // !1 !!!!!!!!!!!!!!!!!!!!!!
    }
  //Ubi End.
  return Num_CD_Drive;			// useless...
}


int
ASPI_End (void)
{
  if (FD_CDROM == -1)
    return -1;
  close (FD_CDROM);
  Num_CD_Drive = 0;
  //By Ubi
  return 0;
}


void
ASPI_Reset_Drive (char *buf)
{
  Read_Complete = 1;

  ASPI_Star_Stop_Unit (STOP_DISC, NULL);
  ASPI_Star_Stop_Unit (CLOSE_TRAY, NULL);
  ASPI_Test_Unit_Ready (7000);
  ASPI_Star_Stop_Unit (START_DISC, NULL);

  // Fill the TOC and set the CD_Present flag
  ASPI_Read_TOC (1, 0, 0, 0, NULL);

  if (CD_Present)
    {
      while (ASPI_Read_CD_LBA (0, 1, 0, 0, 0, 0, NULL) == -1);
    }
  else
    ASPI_Read_CD_LBA (0, 1, 0, 0, 0, 0, NULL);

  memcpy (buf, &Buf_Read[0x100], 0x200);
  CDD_Complete = 1;
}


int
ASPI_Test_Unit_Ready (int timeout) //da verificare
{
  int wait_time = 0;

  CDD_Complete = 1;
  while (wait_time < timeout)
  {
	int ret = ioctl(FD_CDROM, CDROM_DRIVE_STATUS, CDSL_CURRENT);
		switch(ret) {
			case CDS_DISC_OK:
				return 0;
			case CDS_NO_DISC:
				return 2;
			case CDS_TRAY_OPEN:
				return 3;
		}
    wait_time += 10;
    Sleep (8);
  }

  return -1;
}


int
ASPI_Lock (int flock)
{
  int ret;			//By Ubi Start
  ret = ioctl (FD_CDROM, CDROM_LOCKDOOR, flock);
  CDD_Complete = 1;
  return 0;
  //By Ubi End, nothing to make.
}


int
ASPI_Star_Stop_Unit (int op, int (*PostProc) (struct tagSRB32_ExecSCSICmd *))
{
  SRB_ExecSCSICmd s_loc, *s;
//By Ubi Start
  int ret;
  switch (op)
    {
    case START_DISC:
      ret = ioctl (FD_CDROM, CDROMSTART, 0);
      break;
    case STOP_DISC:
      ret = ioctl (FD_CDROM, CDROMSTOP, 0);
      break;
    case CLOSE_TRAY:
      ret = ioctl (FD_CDROM, CDROMCLOSETRAY, 0);
      break;
    case OPEN_TRAY:
      ret = ioctl (FD_CDROM, CDROMEJECT, 0);
      break;
    default:
      ret = -1;
    }
  if (ret < 0)
    return 0;			//What i return with an error?
  if (PostProc == NULL)
    PostProc = ASPI_Star_Stop_Unit_COMP;
  s_loc.SRB_Status = SS_COMP;
  return PostProc (&s_loc);
  return 0;
  //End
}


void
LINUXCD_Select_Speed ()
{
	int ret = -1;
	int speed[6] = {20,12,10,8,4,0};
	int i = 0;

	if(CDROM_SPEED)
	{
		ret = ioctl(FD_CDROM, CDROM_SELECT_SPEED, CDROM_SPEED);
	}

	while(ret < 0 && i < 6)
	{
		ret = ioctl(FD_CDROM, CDROM_SELECT_SPEED, speed[i]);
		i++;
	}
}


int
ASPI_Read_TOC (int MSF, int format, int st, int async,
	       int (*PostProc) (struct tagSRB32_ExecSCSICmd *))
{
  SRB_ExecSCSICmd s_loc, *s;
  //By Ubi Start:
  int ret, i, t;
  struct cdrom_tochdr tochd;
  struct cdrom_tocentry tocent;


  ret = ioctl (FD_CDROM, CDROMREADTOCHDR, &tochd);
  if (ret >= 0)
    {
      toc.tocLen = sizeof (toc);
      toc.firstTrack = tochd.cdth_trk0;
      toc.lastTrack = tochd.cdth_trk1;

      for (t = 0, i = tochd.cdth_trk0; i <= tochd.cdth_trk1; i++, t++)
	{
	  tocent.cdte_track = i;
	  tocent.cdte_format = CDROM_MSF;
	  ret = ioctl (FD_CDROM, CDROMREADTOCENTRY, &tocent);
	  toc.tracks[t].ADR = tocent.cdte_ctrl | tocent.cdte_datamode;
	  toc.tracks[t].trackNumber = i;
	  toc.tracks[t].addr[1] = tocent.cdte_addr.msf.minute;
	  toc.tracks[t].addr[2] = tocent.cdte_addr.msf.second;
	  toc.tracks[t].addr[3] = tocent.cdte_addr.msf.frame;
	  toc.tracks[t].addr[0] = 0x01;
	}
      tocent.cdte_track = 0xAA;	//Lead Out!!!
      tocent.cdte_format = CDROM_MSF;
      ret = ioctl (FD_CDROM, CDROMREADTOCENTRY, &tocent);
      toc.tracks[t].ADR = tocent.cdte_ctrl | tocent.cdte_datamode;
      toc.tracks[t].trackNumber = i;
      toc.tracks[t].addr[1] = tocent.cdte_addr.msf.minute;
      toc.tracks[t].addr[2] = tocent.cdte_addr.msf.second;
      toc.tracks[t].addr[3] = tocent.cdte_addr.msf.frame;
      toc.tracks[t].addr[0] = 0x01;
      MAX_LBA = MSF2LBA (tocent.cdte_addr.msf);
      Fill_SCD_TOC_from_MSF_TOC ();
#ifdef DEBUG_CD
      fprintf (debug_SCD_file, "Read TOC: MAX_LBA=%d\n", MAX_LBA);
#endif
    }
  CDD_Complete = 1;
  return 0;
  //By Ubi End.
}


int
ASPI_Stop_Play_Scan (int async,
		     int (*PostProc) (struct tagSRB32_ExecSCSICmd *))
{
  SRB_ExecSCSICmd s_loc, *s;

 CDD_Complete = 1;
  if (PostProc)
    return PostProc (s);
  return 0;			//By Ubi: nothing to make.
}


int
ASPI_Seek (int pos, int async,
	   int (*PostProc) (struct tagSRB32_ExecSCSICmd *))
{
  SRB_ExecSCSICmd s_loc, *s;

  //By Ubi
  if (pos < 0)
    pos = 0;
  if (pos >= MAX_LBA)
    return -1;
  LBA_POS = pos;
  CDD_Complete = 1;
  s_loc.SRB_Status = SS_COMP;
  if (PostProc == NULL)
    PostProc = ASPI_Seek_COMP;
  return PostProc (&s_loc);
  return 0;			//Error -1;
  //By Ubi End.
}


int
ASPI_Read_CD_LBA (int adr, int lenght, unsigned char sector,
		  unsigned char flag, unsigned char sub_chan, int async,
		  int (*PostProc) (struct tagSRB32_ExecSCSICmd *))
{
  SRB_ExecSCSICmd s_loc, *s;
  //By Ubi: start.
  int ret;
  int lbi;
  union cdrd cdread;

  if (lenght > 64)
    lenght = 64;
  else if (lenght <= 0)
    lenght = 1;
  if (adr < 0)
    adr = 0;
  for (lbi = 0; lbi < lenght; lbi++)
    {
      cdread.msf = LBA2MSF (adr + lbi);
      ret = ioctl (FD_CDROM, CDROMREADRAW, &cdread);
      if (ret < 0)
	return 2;
      memcpy (Buf_Read + 2366 * lbi, cdread.buf + 16, 2048);
      //Falta el subchanel al final;
    }
  CDD_Complete = 1;
  return 0;
  //By Ubi End.
}


/*********************************
 *	Default Callbacks functions  *
 *********************************/


int
ASPI_Star_Stop_Unit_COMP (SRB_ExecSCSICmd * s)
{
  ASPI_Command_Running = 0;

  if (s->SRB_Status == SS_COMP)
    {
      if (s->SRB_TargStat == STATUS_GOOD)
	return 0;
      else
	return 2;
    }

  return -1;
}


int
ASPI_Stop_Play_Scan_COMP (SRB_ExecSCSICmd * s)
{
  ASPI_Command_Running = 0;

  if (s->SRB_Status == SS_COMP)
    {
      return 0;
    }

  return -1;
}


int
ASPI_Seek_COMP (SRB_ExecSCSICmd * s)
{
  ASPI_Command_Running = 0;

  if (s->SRB_Status == SS_COMP)
    {
      return 0;
    }

  return -1;
}



/********************************
 *	Custom Callbacks functions  *
 ********************************/


int
ASPI_Stop_CDD_c1_COMP (SRB_ExecSCSICmd * s)
{
  ASPI_Command_Running = 0;

  if (CD_Present)
    SCD.Status_CDD = STOPPED;
  else
    SCD.Status_CDD = NOCD;
  CDD.Status = 0x0000;

  CDD.Control |= 0x0100;	// Data bit set because stopped

  CDD.Minute = 0;
  CDD.Seconde = 0;
  CDD.Frame = 0;
  CDD.Ext = 0;

  CDD_Complete = 1;
  return 0;
}


int
ASPI_Fast_Seek_COMP (SRB_ExecSCSICmd * s)
{
  ASPI_Command_Running = 0;

  return 0;
}


int
ASPI_Seek_CDD_c4_COMP (SRB_ExecSCSICmd * s)
{
  ASPI_Command_Running = 0;

  SCD.Status_CDD = READY;
  CDD.Status = 0x0200;

  if (SCD.TOC.Tracks[SCD.Cur_Track - SCD.TOC.First_Track].Type)
    CDD.Control |= 0x0100;
  else
    CDD.Control &= 0xFEFF;	// Data bit cleared because audio outputed

  CDD.Minute = 0;
  CDD.Seconde = 0;
  CDD.Frame = 0;
  CDD.Ext = 0;

  CDD_Complete = 1;

  if (s->SRB_Status == SS_COMP)
    {
      return 0;
    }
  else
    {
      return -1;
    }
}


int
ASPI_Close_Tray_CDD_cC_COMP (SRB_ExecSCSICmd * s)
{
  ASPI_Command_Running = 0;

  if (s->SRB_Status == SS_COMP)
    {
      SCD.Status_CDD = STOPPED;
      CDD.Status = 0x0000;

      CDD.Minute = 0;
      CDD.Seconde = 0;
      CDD.Frame = 0;
      CDD.Ext = 0;

      CDD_Complete = 1;
      return 0;
    }
  else
    {
      CDD.Status = (CDD.Status & 0xFF) | SCD.Status_CDD;

      CDD.Minute = 0;
      CDD.Seconde = 0;
      CDD.Frame = 0;
      CDD.Ext = 0;

      CDD_Complete = 1;
      return -1;
    }
}


int
ASPI_Open_Tray_CDD_cD_COMP (SRB_ExecSCSICmd * s)
{
  ASPI_Command_Running = 0;

  if (s->SRB_Status == SS_COMP)
    {
      SCD.Status_CDD = TRAY_OPEN;
      CDD.Status = 0x0E00;

      CD_Present = 0;

      CDD.Minute = 0;
      CDD.Seconde = 0;
      CDD.Frame = 0;
      CDD.Ext = 0;

      CDD_Complete = 1;
      return 0;
    }
  else
    {
      CDD.Status = (CDD.Status & 0xFF) | SCD.Status_CDD;

      CDD.Minute = 0;
      CDD.Seconde = 0;
      CDD.Frame = 0;
      CDD.Ext = 0;

      CDD_Complete = 1;
      return -1;
    }
}


/*******************************
 *        CDC functions        *
 *******************************/


void
ASPI_Read_One_LBA_CDC (void)
{
  int lba_to_read, wr_adr;
  //By Ubi: start.
  int ret;
  union cdrd cdread;
  struct cdrom_msf0 msf;
  //By Ubi End.

  if (SCD.Cur_Track == 100)
    goto end;

  lba_to_read = SCD.Cur_LBA;
  if (lba_to_read < 0)
    lba_to_read = 0;
  if (lba_to_read > MAX_LBA)
    lba_to_read = MAX_LBA - 1;
  //Start Ubi
  wr_adr = (CDC.PT.N + 2352) & 0x7FFF;
  /*if ((SCD.Status_CDC & 1) == 0)
    {
      Read_Complete = 1;
      CDD_Complete = 1;
      goto end;			// if read stopped return...
    }*/
  msf = LBA2MSF (SCD.Cur_LBA);
  //msf = LBA2MSF (lba_to_read);
  cdread.msf = LBA2MSF (lba_to_read);
  ret = ioctl (FD_CDROM, CDROMREADRAW, &cdread);
  //            if (ret<0) deberia devolver un error. quizas...
  //Falta el subchanel al final;
#ifdef DEBUG_CD
  fprintf (debug_SCD_file, "Sector %d in CDC.Buf[%d]\n", lba_to_read, wr_adr);
#endif
  CDC_Update_Header ();

  if (CDD.Control & 0x0100)	// DATA
    {
      if (CDC.CTRL.B.B0 & 0x80)	// DECEN = decoding enable
	{
	  if (CDC.CTRL.B.B0 & 0x04)	// WRRQ : this bit enable write to buffer
	    {
              SCD.Cur_LBA++;

	      CDC.WA.N = (CDC.WA.N + 2352) & 0x7FFF;	// add one sector to WA
	      CDC.PT.N = (CDC.PT.N + 2352) & 0x7FFF;	// add one sector to PT

              memcpy (&CDC.Buffer[CDC.PT.N + 4], cdread.buf + 16, 2048);
              CDC.Buffer[CDC.PT.N + 0] = INT_TO_BCDB (msf.minute);
	      CDC.Buffer[CDC.PT.N + 1] = INT_TO_BCDB (msf.second);
	      CDC.Buffer[CDC.PT.N + 2] = INT_TO_BCDB (msf.frame);
	      CDC.Buffer[CDC.PT.N + 3] = 1;
	    }

          CDC.STAT.B.B0 = 0x80;

	  if (CDC.CTRL.B.B0 & 0x10)	// determine form bit form sub header ?
	    {
	      CDC.STAT.B.B2 = CDC.CTRL.B.B1 & 0x08;
	    }
	  else
	    {
	      CDC.STAT.B.B2 = CDC.CTRL.B.B1 & 0x0C;
	    }

	  if (CDC.CTRL.B.B0 & 0x02)
	    CDC.STAT.B.B3 = 0x20;	// ECC done
	  else
	    CDC.STAT.B.B3 = 0x00;	// ECC not done

	  if (CDC.IFCTRL & 0x20)
	    {
	      if (Int_Mask_S68K & 0x20)
		sub68k_interrupt (5, -1);
	      CDC.IFSTAT &= ~0x20;	// DEC interrupt happen
	      CDC_Decode_Reg_Read = 0;	// Reset read after DEC int
	    }
	}
    }
  else				// AUDIO
    {
      SCD.Cur_LBA++;		// Always increment sector if audio

      CDC.WA.N = (CDC.WA.N + 2352) & 0x7FFF;	// add one sector to WA
      CDC.PT.N = (CDC.PT.N + 2352) & 0x7FFF;	// add one sector to PT

      memcpy (&CDC.Buffer[wr_adr], cdread.buf, 2352);
       if (SCD.Cur_LBA > 0)
	{
          memcpy (&CDC.Buffer[wr_adr], cdread.buf, 2352);
	  Write_CD_Audio ((short *) &CDC.Buffer[CDC.PT.N], 44100, 2, 588);
	}

      if (CDC.CTRL.B.B0 & 0x80)	// DECEN = decoding enable
	{
	  CDC.STAT.B.B0 = 0x80;

	  if (CDC.CTRL.B.B0 & 0x10)	// determine form bit form sub header ?
	    {
	      CDC.STAT.B.B2 = CDC.CTRL.B.B1 & 0x08;
	    }
	  else
	    {
	      CDC.STAT.B.B2 = CDC.CTRL.B.B1 & 0x0C;
	    }

	  if (CDC.CTRL.B.B0 & 0x02)
	    CDC.STAT.B.B3 = 0x20;	// ECC done
	  else
	    CDC.STAT.B.B3 = 0x00;	// ECC not done

	  if (CDC.IFCTRL & 0x20)
	    {
	      if (Int_Mask_S68K & 0x20)
		sub68k_interrupt (5, -1);

#ifdef DEBUG_CD
	      fprintf (debug_SCD_file, "CDC - DEC interrupt\n");
#endif

	      CDC.IFSTAT &= ~0x20;	// DEC interrupt happen
	      CDC_Decode_Reg_Read = 0;	// Reset read after DEC int
	    }
	}
    }

  end:

  Read_Complete = 1; // aggiunta
  CDD_Complete = 1;
}


void
Wait_Read_Complete (void)
{
  int i = 0;

  while (Read_Complete == 0)
    Sleep (1);

#ifdef DEBUG_CD
  fprintf (debug_SCD_file, "\n******* Wait Read %d ******\n", i);
#endif
}


/*******************************
 *     Specials Functions      *
 *******************************/


void
Fill_SCD_TOC_from_MSF_TOC (void)
{
  TOCTRACK *t;
  unsigned char i, numTracks;

  SCD.TOC.First_Track = toc.firstTrack;
  SCD.TOC.Last_Track = toc.lastTrack;

#ifdef DEBUG_CD
  fprintf (debug_SCD_file,
	   "\nSCD TOC : First track = %d    Last track = %d\n",
	   SCD.TOC.First_Track, SCD.TOC.Last_Track);
#endif

  numTracks = toc.lastTrack - toc.firstTrack + 1;

  for (i = 0; i < numTracks; i++)
    {
      t = &(toc.tracks[i]);

      SCD.TOC.Tracks[i].Num = t->trackNumber;
      SCD.TOC.Tracks[i].Type = (t->ADR & 0x04) >> 2;	// DATA flag

      SCD.TOC.Tracks[i].MSF.M = t->addr[1];
      SCD.TOC.Tracks[i].MSF.S = t->addr[2];
      SCD.TOC.Tracks[i].MSF.F = t->addr[3];

#ifdef DEBUG_CD
      fprintf (debug_SCD_file, "Track %i - %02d:%02d:%02d ",
	       SCD.TOC.Tracks[i].Num, SCD.TOC.Tracks[i].MSF.M,
	       SCD.TOC.Tracks[i].MSF.S, SCD.TOC.Tracks[i].MSF.F);
      if (SCD.TOC.Tracks[i].Type)
	fprintf (debug_SCD_file, "DATA\n");
      else
	fprintf (debug_SCD_file, "AUDIO\n");
#endif
    }

  SCD.TOC.Tracks[i].Num = toc.tracks[numTracks].trackNumber;
  SCD.TOC.Tracks[i].Type = 0;

  SCD.TOC.Tracks[i].MSF.M = toc.tracks[i].addr[1];
  SCD.TOC.Tracks[i].MSF.S = toc.tracks[i].addr[2];
  SCD.TOC.Tracks[i].MSF.F = toc.tracks[i].addr[3];

#ifdef DEBUG_CD
  fprintf (debug_SCD_file, "End CD - %02d:%02d:%02d\n\n",
	   SCD.TOC.Tracks[numTracks].MSF.M, SCD.TOC.Tracks[numTracks].MSF.S,
	   SCD.TOC.Tracks[numTracks].MSF.F);
#endif

  CD_Present = 1;
}


void
Fill_SCD_TOC_Zero (void)
{
  int i;

  SCD.TOC.First_Track = 0;
  SCD.TOC.Last_Track = 0;

#ifdef DEBUG_CD
  fprintf (debug_SCD_file, "\nSCD TOC filled with zero, CD no present\n");
#endif

  for (i = 0; i < 10; i++)
    {
      SCD.TOC.Tracks[i].Num = 0;
      SCD.TOC.Tracks[i].Type = 0;
      SCD.TOC.Tracks[i].MSF.M = 0;
      SCD.TOC.Tracks[i].MSF.S = 0;
      SCD.TOC.Tracks[i].MSF.F = 0;
    }

  CD_Present = 0;
}
