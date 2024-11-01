<?php
define ('SRAF_PLUGIN_NAME' , 'Super Refer A Friend');
define ('SRAF_PLUGIN_DIR_NAME' , 'super-refer-a-friend');
define ('SRAF_PLUGIN_FILE_NAME' , 'super-refer-a-friend.php');
define ('SRAF_PLUGIN_DIR' , WP_PLUGIN_DIR . '/'. SRAF_PLUGIN_DIR_NAME);
define ('SRAF_INCLUDES_DIR' , SRAF_PLUGIN_DIR . '/includes');
define ('SRAF_CLASS_DIR' , SRAF_INCLUDES_DIR . '/classes');

define ('SRAF_PLUGIN_URL' , WP_PLUGIN_URL . '/' . SRAF_PLUGIN_DIR_NAME);

// Sponsor link text and link
define ('SRAF_SPONSOR_LINK' , 'Super Refer-A-Friend Plugin by <a href="http://wplift.com/">WPLift</a>');

define ('PLUGIN_NAME_PREFIX' , 'sraf');

// Define the default options of the widget
define ('SRAF_DEFAULT_TITLE' , 'Refer a friend');
define ('SRAF_DEFAULT_CAPTCHA_CHAR_NUM' , '5');
define ('SRAF_DEFAULT_MAX_NUM_OF_EMAIL' , '1');
define ('SRAF_DEFAULT_MSG_BODY' , "Hi,\n\nI have found this article {title} at {link} and thought Id share.\n\nThanks,\n-- {Your Name}");
define ('SRAF_DEFAULT_MSG_BODY_LEN' , '1000');
define ('SRAF_DEFAULT_EMAIL_SEND_INTERVAL' , 5);


// Option names
define ('SRAF_WIDGET_TITLE_OPT' , 'sraf_widget_title');
define ('SRAF_WIDGET_CAPTCHA_CHAR_NUM_OPT' , 'sraf_widget_captcha_char_num');
define ('SRAF_WIDGET_MAX_NUM_OF_EMAIL_OPT' , 'sraf_widget_max_num_of_email');
define ('SRAF_WIDGET_MSG_BODY_OPT' , 'sraf_widget_msg_body');
define ('SRAF_WIDGET_MSG_BODY_MAX_LEN_OPT' , 'sraf_widget_msg_body_max_len');
define ('SRAF_WIDGET_EMAIL_SEND_INTERVAL_OPT' , 'sraf_widget_email_send_interval');

// Validation limits. Some limits are defined by admin.
define ('SRAF_MAX_NAME_LENGTH', 100);
define ('SRAF_MAX_EMAIL_LENGTH', 100);
define ('SRAF_MIN_CAPTCHA_CHAR_NUM' , 4);
define ('SRAF_MAX_CAPTCHA_CHAR_NUM' , 10);
define ('SRAF_MIN_MAX_NUM_OF_EMAIL' , 1);
define ('SRAF_MAX_MAX_NUM_OF_EMAIL' , 10);
define ('SRAF_MIN_MSG_BODY_LEN' , 1);
define ('SRAF_MAX_MSG_BODY_LEN' , 5000);
define ('SRAF_MAX_EMAIL_SEND_INTERVAL' , 60 /* 60 minutes */);
define ('SRAF_MIN_EMAIL_SEND_INTERVAL' , 1 /* 1 minute */);

// Thickbox.
define ('SRAF_THICKBOX_WIDTH', 350);
define ('SRAF_THICKBOX_HEIGHT', 380);
