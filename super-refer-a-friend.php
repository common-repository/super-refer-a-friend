<?php
/*
Plugin Name: Super Refer A Friend
Version: 1.0
Description: An advanced refer-a-friend plugin with extra features and customization.
Plugin URI: http://wplift.com/super-refer-a-friend-plugin/
Author: Oliver Dale
Author URI: http://wplift.com
*/

/*
Super Refer A Friend is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Super Refer A Friend is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with Super Refer A Friend.  If not, see <http://www.gnu.org/licenses/>.
*/


require_once('includes/sraf-config.inc');
require_once('includes/sraf-validator.inc');
require_once('includes/sraf-common.inc');
require_once('includes/sraf-widget.inc');

$sraf_common = new SRAFCommon();
$sraf_widget = new SRAFWidget();

register_activation_hook(__FILE__, array(&$sraf_widget, 'activated'));
register_deactivation_hook(__FILE__, array(&$sraf_widget, 'deactivated'));

add_action('widgets_init', array(&$sraf_widget, 'init'));
add_action('wp_print_styles', array(&$sraf_common, 'addStyleSheets'));
add_action('admin_print_styles', array(&$sraf_common, 'addAdminStyleSheets'));
add_action('wp_print_scripts', array(&$sraf_common, 'addScripts'));
add_action('admin_print_scripts', array(&$sraf_common, 'addAdminScripts'));
add_action('wp_footer', array(&$sraf_common, 'addSponsorLinks'));
