/*
 *  gsetroot - gtk-based front-end for Esetroot
 *  Copyright (C) 2005 Bellabes Lounis
 *  nolius@users.sourceforge.net
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License as published by
 *  the Free Software Foundation; either version 2 of the License, or
 *  (at your option) any later version.
 *
 *  This program is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  GNU Library General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <gtk/gtk.h>

#include <stdlib.h>
#include <string.h>
#include "callbacks.h"
#include "interface.h"
#include "support.h"


#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)



char command[1000];

void
on_button_open_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget *dialog;

	dialog = gtk_file_chooser_dialog_new ("Open File",
				      NULL,
				      GTK_FILE_CHOOSER_ACTION_OPEN,
				      GTK_STOCK_CANCEL, GTK_RESPONSE_CANCEL,
				      GTK_STOCK_OPEN, GTK_RESPONSE_ACCEPT,
				      NULL);

	if (gtk_dialog_run (GTK_DIALOG (dialog)) == GTK_RESPONSE_ACCEPT)
	{
		char *filename;
		
		filename = gtk_file_chooser_get_filename (GTK_FILE_CHOOSER (dialog));
		
		GtkWidget * entry = lookup_widget(GTK_WIDGET(button), "entry_image");
		gtk_entry_set_text (GTK_ENTRY (entry), filename);
		
		// preview
		GtkWidget * im = lookup_widget(GTK_WIDGET(button), "image");
		gtk_image_set_from_file (GTK_IMAGE (im), filename);
		
		
		free (filename);
	}
		
	gtk_widget_destroy (dialog);


}


void
on_button_apply_clicked                (GtkButton       *button,
                                        gpointer         user_data)
{
	
	strcpy(command, "");
	
	GtkWidget * radiobutton_center = lookup_widget(GTK_WIDGET(button), "radiobutton_center");
	GtkWidget * radiobutton_screensize = lookup_widget(GTK_WIDGET(button), "radiobutton_screensize");
	GtkWidget * radiobutton_mosaic = lookup_widget(GTK_WIDGET(button), "radiobutton_mosaic");
	
	//GTK_RADIOBUTTON(radiobutton_center)
	
	if(gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_center)))
	{
		strcpy(command, "Esetroot -c ");
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_screensize)))
	{
		strcpy(command, "Esetroot -s ");
	}
	else if (gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(radiobutton_mosaic)))
	{
		strcpy(command, "Esetroot ");
	}

	GtkWidget * entry = lookup_widget(GTK_WIDGET(button), "entry_image");
	strcat(command, "\"");
	strcat(command, gtk_entry_get_text (GTK_ENTRY (entry)));
	strcat(command, "\"");

	system(command);
	
	
}


void
on_button_preview_clicked              (GtkButton       *button,
                                        gpointer         user_data)
{
	GtkWidget * entry = lookup_widget(GTK_WIDGET(button), "entry_image");	
	GtkWidget * im = lookup_widget(GTK_WIDGET(button), "image");
	
	gtk_image_set_from_file (GTK_IMAGE (im), gtk_entry_get_text (GTK_ENTRY (entry)));
	
}

void
on_button_info_clicked                 (GtkButton       *button,
                                        gpointer         user_data)
{
  GtkWidget *window_info;
  GtkWidget *fixed1;
  GtkWidget *label1;
  GtkWidget *label3;
  GtkWidget *label4;
  GtkWidget *label6;
  GtkWidget *label5;
  GtkWidget *label2;

  window_info = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_window_set_title (GTK_WINDOW (window_info), _("infos"));
  gtk_window_set_position (GTK_WINDOW (window_info), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size (GTK_WINDOW (window_info), 600, 300);
  gtk_window_set_resizable (GTK_WINDOW (window_info), FALSE);

  fixed1 = gtk_fixed_new ();
  gtk_widget_show (fixed1);
  gtk_container_add (GTK_CONTAINER (window_info), fixed1);

  label1 = gtk_label_new (_("gsetroot 1.1 is a gtk-based front-end for Esetroot "));
  gtk_widget_show (label1);
  gtk_fixed_put (GTK_FIXED (fixed1), label1, 0, 8);
  gtk_widget_set_size_request (label1, 354, 24);

  label3 = gtk_label_new (_("Use it under FluxBox, Enlightenment, WindowMaker,"));
  gtk_widget_show (label3);
  gtk_fixed_put (GTK_FIXED (fixed1), label3, 0, 32);
  gtk_widget_set_size_request (label3, 354, 24);

  label4 = gtk_label_new (_("BlackBox, and others"));
  gtk_widget_show (label4);
  gtk_fixed_put (GTK_FIXED (fixed1), label4, 0, 56);
  gtk_widget_set_size_request (label4, 354, 24);

  label6 = gtk_label_new (_("------------------------------------"));
  gtk_widget_show (label6);
  gtk_fixed_put (GTK_FIXED (fixed1), label6, 0, 80);
  gtk_widget_set_size_request (label6, 354, 16);

  label5 = gtk_label_new (_("nolius@users.sourceforge.net"));
  gtk_widget_show (label5);
  gtk_fixed_put (GTK_FIXED (fixed1), label5, 0, 120);
  gtk_widget_set_size_request (label5, 354, 24);

  label2 = gtk_label_new (_("By Lounis Bellabes"));
  gtk_widget_show (label2);
  gtk_fixed_put (GTK_FIXED (fixed1), label2, 0, 96);
  gtk_widget_set_size_request (label2, 354, 24);

  GLADE_HOOKUP_OBJECT_NO_REF (window_info, window_info, "window_info");
  GLADE_HOOKUP_OBJECT (window_info, fixed1, "fixed1");
  GLADE_HOOKUP_OBJECT (window_info, label1, "label1");
  GLADE_HOOKUP_OBJECT (window_info, label3, "label3");
  GLADE_HOOKUP_OBJECT (window_info, label4, "label4");
  GLADE_HOOKUP_OBJECT (window_info, label6, "label6");
  GLADE_HOOKUP_OBJECT (window_info, label5, "label5");
  GLADE_HOOKUP_OBJECT (window_info, label2, "label2");

  gtk_widget_show_all(window_info);


}

void
on_window1_remove                      (GtkContainer    *container,
                                        GtkWidget       *widget,
                                        gpointer         user_data)
{
 	gtk_main_quit ();
}
