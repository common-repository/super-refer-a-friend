<?php

if(!empty($_POST['sraf_submit'])) {
	if(!empty($_POST['sraf_linkback'])) {
		delete_option('sraf_linkback');
		add_option('sraf_linkback', '2');
	}
	else {
		delete_option('sraf_linkback');
		add_option('sraf_linkback', '1');
	}
}

class SRAFWidget {
	var $sraf_common;
	var $before_widget;
	/**
	 *	Constructor
	 */
	function SRAFWidget() {
		$this->sraf_common = new SRAFCommon();
	}
	
	/**
	 * 	Initialize the plugin by adding it as widget and adding its settings.
	 */
	function init() {
		// Register widget for use
		register_sidebar_widget(array(SRAF_PLUGIN_NAME, 'widgets'), array($this, '_getWidget'), 'widget');		
		// Register settings form
		register_widget_control(array(SRAF_PLUGIN_NAME, 'widgets'), array($this, '_getWidgetControl'));
		
	}

	function activated() {
		$this->_getWidgetActivated();
		$this->_installSchema();
	}

	function deactivated() {
		$this->_getWidgetDeactivated();
		$this->_unInstallSchema();
	}
	
	///////////////////////////////////////////////////////////////////////////
	// Restricted Stuffs.

	/**
	 * Frontend viewer of the widget.
	 *
	 * Anything the function prints, will be shown in frontend.
	 */
	function _getWidget($args) {
		extract($args);
			
		$str = '';
		$str .= $before_widget;
		
		$str .= '<ul class="sraf-container widget">';
		$str .= 	'<div class="sidebartitle">';
		$str .=    		$before_title. get_option(SRAF_WIDGET_TITLE_OPT) . $after_title;
		$str .= 		'<div class="friend-emails">';
		$str .=     		'<form method="POST" id="friend_emails_form" onsubmit="return false;">';
		$str .=     			$this->_getFriendsEmailFormInitial(1);
		$str .=    				'<a class="thickbox anchor-button friend-emails-submit-button"' .
									' href="#TB_inline?height=' . SRAF_THICKBOX_HEIGHT . '&width=' . SRAF_THICKBOX_WIDTH . '&inlineId=sraf_form_container"' .
								'>Send</a>';
		$str .=     		'</form>';
		$str .= 		'</div>';
		$str .= 	'</div>';
		$str .= 	'<div id="sraf_form_container" class="sidebarcontent invisible">';
		$str .=			$this->_getReferingForm();
		$str .= 	'</div>';
		
		$str .= 	'<script type="text/JavaScript">';
		$str .= 		'var sraf_captcha_img_url = "' . SRAF_PLUGIN_URL . '/sraf-captcha.php?width=200&height=30&characters=' . $this->sraf_common->getMaxCaptchaCharNum() . '";';
		$str .= 	'</script>';
		
		$str .= '</ul>';
		
		$str .= $after_widget;

		print $str;
	}

	/**
	 * Backend controller for the widget settings.
	 *
	 * Shows the widget option form and saves the settings accordingly when the
	 * form is submitted
	 */
	function _getWidgetControl() {
		// REMEMBER: Wordpress's JS will bring the vaue of the field "input[id*='-title']"
		//           in the wigdet title.
		
		$controlOptions = array(
			SRAF_WIDGET_TITLE_OPT => array(
				'#type' => 'text', '#title' => 'Title', '#name' => 'title'
			),
			SRAF_WIDGET_CAPTCHA_CHAR_NUM_OPT => array(
				'#type' => 'text', '#title' => 'Captcha Char Num', '#name' => 'captcha_char_num',
				'#size' => '3', '#rule' => array(
					'type' => 'integer',
					'min' => SRAF_MIN_CAPTCHA_CHAR_NUM,
					'max' => SRAF_MAX_CAPTCHA_CHAR_NUM,
					'default-value' => intval(SRAF_DEFAULT_CAPTCHA_CHAR_NUM)
				),
				'#suffix' => $this->_getRangeLabel(SRAF_MIN_CAPTCHA_CHAR_NUM, SRAF_MAX_CAPTCHA_CHAR_NUM),
			),
			SRAF_WIDGET_MAX_NUM_OF_EMAIL_OPT => array(
				'#type' => 'text', '#title' => 'Max Friend Emails', '#name' => 'max_friend',
				'#size' => '3', '#rule' => array(
					'type' => 'integer',
					'min' => SRAF_MIN_MAX_NUM_OF_EMAIL,
					'max' => SRAF_MAX_MAX_NUM_OF_EMAIL,
					'default-value' => intval(SRAF_DEFAULT_MAX_NUM_OF_EMAIL)
				),
				'#suffix' => $this->_getRangeLabel(SRAF_MIN_MAX_NUM_OF_EMAIL, SRAF_MAX_MAX_NUM_OF_EMAIL),
			),
			SRAF_WIDGET_MSG_BODY_OPT => array(
				'#type' => 'textarea', '#title' => 'Message Body', '#name' => 'msg_body',
				'#rows' => '10', '#cols' => '25'
			),
			SRAF_WIDGET_MSG_BODY_MAX_LEN_OPT => array(
				'#type' => 'text', '#title' => 'Max Message Size', '#name' => 'msg_body_max_len',
				'#size' => '5', '#rule' => array(
					'type' => 'integer',
					'min' => SRAF_MIN_MSG_BODY_LEN,
					'max' => SRAF_MAX_MSG_BODY_LEN,
					'default-value' => intval(SRAF_DEFAULT_MSG_BODY_LEN)
				),
				'#suffix' => $this->_getRangeLabel(SRAF_MIN_MSG_BODY_LEN, SRAF_MAX_MSG_BODY_LEN),
			),
			SRAF_WIDGET_EMAIL_SEND_INTERVAL_OPT => array(
				'#type' => 'text', '#title' => 'Max Time to send another Email', '#name' => 'max_send_email_allow_time',
				'#size' => '3', '#rule' => array(
					'type' => 'integer',
					'min' => SRAF_MIN_EMAIL_SEND_INTERVAL,
					'max' => SRAF_MAX_EMAIL_SEND_INTERVAL,
					'default-value' => SRAF_DEFAULT_EMAIL_SEND_INTERVAL
				),
				'#suffix' => '<label class="sraf-range">minutes.</label>',
			)
		);
		
		$str = '';
		
		// Handle form submission.
		if (isset($_POST[PLUGIN_NAME_PREFIX . '_widget_submit'])){
			$this->sraf_common->updatePostedOptions(PLUGIN_NAME_PREFIX, $controlOptions);
		}
		
		// Produce form.
		$str .= '<div class="' . PLUGIN_NAME_PREFIX . '-widget-option-form">';
			$str .= $this->sraf_common->getOptionsForm(PLUGIN_NAME_PREFIX, $controlOptions);
			$str .= '<input type="hidden" name="' . PLUGIN_NAME_PREFIX . '_widget_submit" value="1" />';
			$check = get_option('sraf_linkback');
			if($check != '1')
				$checked = 'checked';
			$str .= '<input type="checkbox" name="sraf_linkback" value="1" '.$checked.'/>Include attribution link?';
			$str .= '<input type="hidden" name="sraf_submit" value="1"/>';
		$str .= '</div>';

		print $str;
	}

	function _getReferingForm() {
		$form = '<div class="sraf-container">';
			$form .='<form method="POST" id="mail_submission_form" action="' . SRAF_PLUGIN_URL . '/sraf_form_process.php">';
				$form .= $this->_getSenderInfoForm();
				$form .= $this->_getFriendsEmailFormAdditional();
				$form .= $this->_getMsgBodyForm();
				$form .= $this->_getCaptchaForm();
				$form .='<input type="button" name="mail-submission-form-submit" class="generic-button send-mail" value="Send" />';
			$form .='</form>';
		$form .='</div>';
		return $form;
	}
	
	///////////////////////////////////////////////////////////////////////////

	/**
	 * 	Add default widget options
	 */
	function _getWidgetActivated() {
		$allOptions = $this->_getOptions();
		foreach ( $allOptions as $optionName => $defaultValue ) {
			$this->sraf_common->setDefaultOptionValue($optionName, $defaultValue);
		}
	}

	/**
	 * 	Delete widget options from database
	 */
	function _getWidgetDeactivated() {
		$allOptions = $this->_getOptions();
		foreach ( $allOptions as $optionName => $defaultValue ) {
			delete_option($optionName);
		}
	}
	/**
	 *	Create table "wp_sraf_ip" for keeping the tract of IP address
	 *
	 */
	function _installSchema() {
		global $wpdb;
		$wpdb->query('CREATE TABLE ' . $this->sraf_common->getIpTableName() .
			'(
				id INT(11) NOT NULL AUTO_INCREMENT PRIMARY KEY,
				ip VARCHAR(30) NOT NULL,
				date DATETIME NOT NULL
			) ENGINE = MYISAM'
		);
	}
	
	/**
	 *	Drop table "wp_sraf_ip".
	 */
	function _unInstallSchema() {
		global $wpdb;
		$wpdb->query('DROP TABLE ' . $this->sraf_common->getIpTableName());
	}
	
	function _getFriendsEmailFormInitial($maxFriend = -1, $options = array()) {
		return (
			'<div class="friend-email-info friend-email-info-initial">' .
				$this->_getFriendsEmailFormLabel($maxFriend) .
				$this->_getFriendsEmailForm($maxFriend, $options) .
			'</div>'
		);
	}
	
	function _getFriendsEmailFormAdditional($maxFriend = -1, $options = array()) {
		return (
			'<table class="friend-email-info friend-email-info-additional" cellspacing="0" cellpadding="0" border="0">' .
				'<tbody>' .
					'<tr>' .
						'<td class="friend-email-title-con">' . $this->_getFriendsEmailFormLabel($maxFriend) . '</td>' .
						'<td>' . $this->_getFriendsEmailForm($maxFriend, $options) . '</td>' .
					'<tr>' .
				'</tbody>' .
			'</table>'
		);
	}
	
	function _getFriendsEmailFormLabel($maxFriend = -1) {
		if ($maxFriend < 0) {
			$maxFriend = $this->sraf_common->getMaxNumOfEmail();
		}
		return '<label>' . __("Friend's E-mail" . ($maxFriend > 1 ? 's' : '')) . '</label>';
	}

	/**
	 * Get the series of text fields for all friends' emails.
	 * 
	 * @param int $maxFriend - Max number of allowed friends. Optional.
	 * @param Map<String, String>[] $options - Options for all friend emails. Optional.
	 */
	function _getFriendsEmailForm($maxFriend = -1, $options = array()) {
		if ($maxFriend < 0) {
			$maxFriend = $this->sraf_common->getMaxNumOfEmail();
		}

		for($i = 1; $i <= $maxFriend; $i++) {
			$wrapperClass = 'friend-email ' . ($i === 1 ? '' : 'invisible');
			$name = 'sraf-friend-email' . $i;
			
			// Initial value to put in the field.
			$value = '';
			if (isset($options[$i - 1]) && isset($options[$i - 1]['value'])) {
				$value = $options[$i - 1]['value'];
			} else {
				$value = $this->sraf_common->getPostedText($name, '');
			}
			
			// Additional attributes to put in the field.
			$disabled = FALSE;
			if (isset($options[$i - 1]) && isset($options[$i - 1]['disabled'])) {
				$disabled = $options[$i - 1]['disabled'];
			}
			
			$form .= (
				'<div class="form-item form-item-textfield ' . $wrapperClass . '">' .
					'<div class="friend-email-field">' .
						'<input' .
							' class="' . ($i === 1 ? 'required' : '') . ' email ' . ($disabled ? 'disabled' : '') . '"' .
							' id="' . $name . '" type="text"' .
							' name="' . $name . '" value="' . $value . '"' .
							' maxLength="' . SRAF_MAX_EMAIL_LENGTH . '"' .
							($disabled ? ' disabled="disabled"' : '') .
							' />' .
						(($i === 1 || $disabled) ? '' : '<span class="remove-friend-email">X</span>') .
					'</div>' .
					'<label class="sraf-error" dynamic="true" for="' . $name . '"></label>' .
				'</div>'
			);
		}
		
		// No "Add new" button for only 1 friend.
		if ($maxFriend > 1) {
			$form .= '<input type="button" class="add-new-email-button" value="Add New" />';
		}
		
		return $form;
	}
	
	function _getSenderInfoForm() {
		$form ='<div class="sender-info">';
			$form .= $this->_getFormText(__('Your name'), 'sraf-sender-name', array('class' => 'required', 'maxLength' => SRAF_MAX_NAME_LENGTH));
			$form .= $this->_getFormText(__('Your email'), 'sraf-sender-email', array('class' => 'required', 'maxLength' => SRAF_MAX_EMAIL_LENGTH));
		$form .='</div>';
		
		return $form;
	}
	
	function _getMsgBodyForm() {
		$msgBody = get_option(SRAF_WIDGET_MSG_BODY_OPT);
		
		if (is_home()) {
			$title = get_bloginfo('name');
			$link = get_bloginfo('url');
		} else {
			$currentPostId = get_the_ID();
			$currentPost = get_post($currentPostId);
			$title = $currentPost->post_title;
			$link = get_permalink($currentPostId);
		}
		
		$msgBody = preg_replace('/\{title\}/', $title, $msgBody);
		$msgBody = preg_replace('/\{link\}/', $link, $msgBody);
		return $this->sraf_common->getFormTextArea(
			__('Message Body'), 'sraf-msg-body', $msgBody, 'sraf-msg-body-container',
			array('class' => 'sraf-msg-body required', 'cols' => 35, 'maxLength' => $this->sraf_common->getMaxMsgBodyLen())
		);
	}
	
	function _getCaptchaForm() {
		$form = $this->_getCaptchaImage();
		$form .= $this->_getFormText(
			__('Write the above text'), 'sraf-captcha-text',
			array('class' => 'required', 'maxLength' => $this->sraf_common->getMaxCaptchaCharNum())
		);
		
		return $form;
	}
	
	function _getCaptchaImage() {
		return (
			'<center>' .
				'<img class="captcha-image" src="" />' .
			'</center>'
		);
	}

	function _getFormText($label, $postedKey, $options = array()) {
		$currentValue = $this->sraf_common->getPostedText($postedKey, '');
		
		$options['wrapper_class'] = 'refer-a-friend-form-item ' . $postedKey . ' ' . (isset($options['wrapper_class']) ? $options['wrapper_class'] : '');
		$options['suffix'] = '<label class="sraf-error" dynamic="true" for="' . $postedKey . '"></label>';
		
		return (
			$this->sraf_common->getFormText($label, $postedKey, $currentValue, $options)
		);
	}
	
	function _getRangeLabel($min, $max) {
		return '<label class="sraf-range">' . $min . ' to ' . $max . '</label>';
	}
	
	function _getOptions() {
		return array(
			SRAF_WIDGET_TITLE_OPT => SRAF_DEFAULT_TITLE,
			SRAF_WIDGET_CAPTCHA_CHAR_NUM_OPT => SRAF_DEFAULT_CAPTCHA_CHAR_NUM,
			SRAF_WIDGET_MAX_NUM_OF_EMAIL_OPT => SRAF_DEFAULT_MAX_NUM_OF_EMAIL,
			SRAF_WIDGET_MSG_BODY_OPT => SRAF_DEFAULT_MSG_BODY,
			SRAF_WIDGET_MSG_BODY_MAX_LEN_OPT => SRAF_DEFAULT_MSG_BODY_LEN,
			SRAF_WIDGET_EMAIL_SEND_INTERVAL_OPT => SRAF_DEFAULT_EMAIL_SEND_INTERVAL,
		);
	}
}
