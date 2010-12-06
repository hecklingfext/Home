/*
 * DO NOT EDIT THIS FILE - it is generated by Glade.
 */

#ifdef HAVE_CONFIG_H
#  include <config.h>
#endif

#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>

#include <gdk/gdkkeysyms.h>
#include <gtk/gtk.h>

#include "callbacks.h"
#include "interface.h"
#include "support.h"

#define GLADE_HOOKUP_OBJECT(component,widget,name) \
  g_object_set_data_full (G_OBJECT (component), name, \
    gtk_widget_ref (widget), (GDestroyNotify) gtk_widget_unref)

#define GLADE_HOOKUP_OBJECT_NO_REF(component,widget,name) \
  g_object_set_data (G_OBJECT (component), name, widget)

GtkWidget*
create_window1 (void)
{
  GtkWidget *window1;
  AtkObject *atko;
  GtkWidget *fixed;
  GtkWidget *scrolledwindow;
  GtkWidget *viewport;
  GtkWidget *image;
  GtkWidget *entry_image;
  GtkWidget *button_open;
  GtkWidget *alignment1;
  GtkWidget *hbox1;
  GtkWidget *image2;
  GtkWidget *label1;
  GtkWidget *button_preview;
  GtkWidget *alignment2;
  GtkWidget *hbox2;
  GtkWidget *image3;
  GtkWidget *label2;
  GtkWidget *label_image;
  GtkWidget *button_apply;
  GtkWidget *alignment3;
  GtkWidget *hbox3;
  GtkWidget *image4;
  GtkWidget *label3;
  GtkWidget *button_info;
  GtkWidget *alignment4;
  GtkWidget *hbox4;
  GtkWidget *image5;
  GtkWidget *label4;
  GtkWidget *radiobutton_center;
  GSList *radiobutton_center_group = NULL;
  GtkWidget *radiobutton_mosaic;
  GtkWidget *radiobutton_screensize;

  window1 = gtk_window_new (GTK_WINDOW_TOPLEVEL);
  gtk_widget_set_size_request (window1, 660, 600);
  gtk_window_set_title (GTK_WINDOW (window1), _("gsetroot"));
  gtk_window_set_position (GTK_WINDOW (window1), GTK_WIN_POS_CENTER);
  gtk_window_set_default_size (GTK_WINDOW (window1), 500, 400);
  gtk_window_set_resizable (GTK_WINDOW (window1), FALSE);

  fixed = gtk_fixed_new ();
  gtk_widget_show (fixed);
  gtk_container_add (GTK_CONTAINER (window1), fixed);

  scrolledwindow = gtk_scrolled_window_new (NULL, NULL);
  gtk_widget_show (scrolledwindow);
  gtk_fixed_put (GTK_FIXED (fixed), scrolledwindow, 10, 10);
  gtk_widget_set_size_request (scrolledwindow, 640, 480);
  gtk_scrolled_window_set_policy (GTK_SCROLLED_WINDOW (scrolledwindow), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);

  viewport = gtk_viewport_new (NULL, NULL);
  gtk_widget_show (viewport);
  gtk_container_add (GTK_CONTAINER (scrolledwindow), viewport);

  image = create_pixmap (window1, NULL);
  gtk_widget_show (image);
  gtk_container_add (GTK_CONTAINER (viewport), image);

  entry_image = gtk_entry_new ();
  gtk_widget_show (entry_image);
  gtk_fixed_put (GTK_FIXED (fixed), entry_image, 48, 506);
  gtk_widget_set_size_request (entry_image, 368, 24);

  button_open = gtk_button_new ();
  gtk_widget_show (button_open);
  gtk_fixed_put (GTK_FIXED (fixed), button_open, 424, 504);
  gtk_widget_set_size_request (button_open, 100, 40);

  alignment1 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment1);
  gtk_container_add (GTK_CONTAINER (button_open), alignment1);

  hbox1 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox1);
  gtk_container_add (GTK_CONTAINER (alignment1), hbox1);

  image2 = gtk_image_new_from_stock ("gtk-directory", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image2);
  gtk_box_pack_start (GTK_BOX (hbox1), image2, FALSE, FALSE, 0);

  label1 = gtk_label_new_with_mnemonic (_("Open"));
  gtk_widget_show (label1);
  gtk_box_pack_start (GTK_BOX (hbox1), label1, FALSE, FALSE, 0);

  button_preview = gtk_button_new ();
  gtk_widget_show (button_preview);
  gtk_fixed_put (GTK_FIXED (fixed), button_preview, 544, 504);
  gtk_widget_set_size_request (button_preview, 100, 40);

  alignment2 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment2);
  gtk_container_add (GTK_CONTAINER (button_preview), alignment2);

  hbox2 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox2);
  gtk_container_add (GTK_CONTAINER (alignment2), hbox2);

  image3 = gtk_image_new_from_stock ("gtk-zoom-100", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image3);
  gtk_box_pack_start (GTK_BOX (hbox2), image3, FALSE, FALSE, 0);

  label2 = gtk_label_new_with_mnemonic (_("Preview"));
  gtk_widget_show (label2);
  gtk_box_pack_start (GTK_BOX (hbox2), label2, FALSE, FALSE, 0);

  label_image = gtk_label_new (_("File :"));
  gtk_widget_show (label_image);
  gtk_fixed_put (GTK_FIXED (fixed), label_image, 8, 508);
  gtk_widget_set_size_request (label_image, 34, 16);

  button_apply = gtk_button_new ();
  gtk_widget_show (button_apply);
  gtk_fixed_put (GTK_FIXED (fixed), button_apply, 544, 552);
  gtk_widget_set_size_request (button_apply, 100, 40);

  alignment3 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment3);
  gtk_container_add (GTK_CONTAINER (button_apply), alignment3);

  hbox3 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox3);
  gtk_container_add (GTK_CONTAINER (alignment3), hbox3);

  image4 = gtk_image_new_from_stock ("gtk-ok", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image4);
  gtk_box_pack_start (GTK_BOX (hbox3), image4, FALSE, FALSE, 0);

  label3 = gtk_label_new_with_mnemonic (_("Apply"));
  gtk_widget_show (label3);
  gtk_box_pack_start (GTK_BOX (hbox3), label3, FALSE, FALSE, 0);

  button_info = gtk_button_new ();
  gtk_widget_show (button_info);
  gtk_fixed_put (GTK_FIXED (fixed), button_info, 424, 552);
  gtk_widget_set_size_request (button_info, 100, 40);

  alignment4 = gtk_alignment_new (0.5, 0.5, 0, 0);
  gtk_widget_show (alignment4);
  gtk_container_add (GTK_CONTAINER (button_info), alignment4);

  hbox4 = gtk_hbox_new (FALSE, 2);
  gtk_widget_show (hbox4);
  gtk_container_add (GTK_CONTAINER (alignment4), hbox4);

  image5 = gtk_image_new_from_stock ("gtk-dialog-info", GTK_ICON_SIZE_BUTTON);
  gtk_widget_show (image5);
  gtk_box_pack_start (GTK_BOX (hbox4), image5, FALSE, FALSE, 0);

  label4 = gtk_label_new_with_mnemonic (_("Infos"));
  gtk_widget_show (label4);
  gtk_box_pack_start (GTK_BOX (hbox4), label4, FALSE, FALSE, 0);

  radiobutton_center = gtk_radio_button_new_with_mnemonic (NULL, _("Center"));
  gtk_widget_show (radiobutton_center);
  gtk_fixed_put (GTK_FIXED (fixed), radiobutton_center, 48, 552);
  gtk_widget_set_size_request (radiobutton_center, 64, 18);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_center), radiobutton_center_group);
  radiobutton_center_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_center));

  radiobutton_mosaic = gtk_radio_button_new_with_mnemonic (NULL, _("Mosaic"));
  gtk_widget_show (radiobutton_mosaic);
  gtk_fixed_put (GTK_FIXED (fixed), radiobutton_mosaic, 328, 552);
  gtk_widget_set_size_request (radiobutton_mosaic, 89, 18);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_mosaic), radiobutton_center_group);
  radiobutton_center_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_mosaic));

  radiobutton_screensize = gtk_radio_button_new_with_mnemonic (NULL, _("Current screen size"));
  gtk_widget_show (radiobutton_screensize);
  gtk_fixed_put (GTK_FIXED (fixed), radiobutton_screensize, 152, 552);
  gtk_widget_set_size_request (radiobutton_screensize, 160, 18);
  gtk_radio_button_set_group (GTK_RADIO_BUTTON (radiobutton_screensize), radiobutton_center_group);
  radiobutton_center_group = gtk_radio_button_get_group (GTK_RADIO_BUTTON (radiobutton_screensize));

  g_signal_connect ((gpointer) window1, "remove",
                    G_CALLBACK (on_window1_remove),
                    NULL);
  g_signal_connect ((gpointer) button_open, "clicked",
                    G_CALLBACK (on_button_open_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_preview, "clicked",
                    G_CALLBACK (on_button_preview_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_apply, "clicked",
                    G_CALLBACK (on_button_apply_clicked),
                    NULL);
  g_signal_connect ((gpointer) button_info, "clicked",
                    G_CALLBACK (on_button_info_clicked),
                    NULL);

  atko = gtk_widget_get_accessible (window1);
  atk_object_set_name (atko, _("window1"));


  /* Store pointers to all widgets, for use by lookup_widget(). */
  GLADE_HOOKUP_OBJECT_NO_REF (window1, window1, "window1");
  GLADE_HOOKUP_OBJECT (window1, fixed, "fixed");
  GLADE_HOOKUP_OBJECT (window1, scrolledwindow, "scrolledwindow");
  GLADE_HOOKUP_OBJECT (window1, viewport, "viewport");
  GLADE_HOOKUP_OBJECT (window1, image, "image");
  GLADE_HOOKUP_OBJECT (window1, entry_image, "entry_image");
  GLADE_HOOKUP_OBJECT (window1, button_open, "button_open");
  GLADE_HOOKUP_OBJECT (window1, alignment1, "alignment1");
  GLADE_HOOKUP_OBJECT (window1, hbox1, "hbox1");
  GLADE_HOOKUP_OBJECT (window1, image2, "image2");
  GLADE_HOOKUP_OBJECT (window1, label1, "label1");
  GLADE_HOOKUP_OBJECT (window1, button_preview, "button_preview");
  GLADE_HOOKUP_OBJECT (window1, alignment2, "alignment2");
  GLADE_HOOKUP_OBJECT (window1, hbox2, "hbox2");
  GLADE_HOOKUP_OBJECT (window1, image3, "image3");
  GLADE_HOOKUP_OBJECT (window1, label2, "label2");
  GLADE_HOOKUP_OBJECT (window1, label_image, "label_image");
  GLADE_HOOKUP_OBJECT (window1, button_apply, "button_apply");
  GLADE_HOOKUP_OBJECT (window1, alignment3, "alignment3");
  GLADE_HOOKUP_OBJECT (window1, hbox3, "hbox3");
  GLADE_HOOKUP_OBJECT (window1, image4, "image4");
  GLADE_HOOKUP_OBJECT (window1, label3, "label3");
  GLADE_HOOKUP_OBJECT (window1, button_info, "button_info");
  GLADE_HOOKUP_OBJECT (window1, alignment4, "alignment4");
  GLADE_HOOKUP_OBJECT (window1, hbox4, "hbox4");
  GLADE_HOOKUP_OBJECT (window1, image5, "image5");
  GLADE_HOOKUP_OBJECT (window1, label4, "label4");
  GLADE_HOOKUP_OBJECT (window1, radiobutton_center, "radiobutton_center");
  GLADE_HOOKUP_OBJECT (window1, radiobutton_mosaic, "radiobutton_mosaic");
  GLADE_HOOKUP_OBJECT (window1, radiobutton_screensize, "radiobutton_screensize");

  return window1;
}
