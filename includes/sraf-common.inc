<?php

class SRAFCommon {
	var $validator;
	
	/**
	 *	The constructor
	 */
	function SRAFCommon() {
		$this->validator = new SRAFValidator();
	}
	
	function getValidator() {
		return $this->validator;
	}
	
	/**
	 * Input / output:
	 *    $tablePrefix: 'wp_link'
	 *    $columnNames: [ 'link_id', 'link_name', 'link_url' ]
	 *         Returns: 'wp_link.link_id, wp_link.link_name, wp_link.link_url'
	 *
	 * @param String $tablePrefix - Name or alias of the table. Mandatory.
	 * @param String[] $columnNames - Column names. Mandatory.
	 */
	function getDBColumns($tablePrefix, $columnNames = array()) {
		$str = '';
		foreach ( $columnNames as $columnName ) {
			if ($str != '') {
				$str .= ', ';
			}
			$str .= $tablePrefix . '.' . $columnName;
		}
		return $str;
	}

	/**
	 * Add necessary stylesheet to the head.
	 */
	function addStyleSheets() {
		wp_register_style('sraf_styles', SRAF_PLUGIN_URL . '/css/sraf.css');
		wp_register_style('thick_box_styles', SRAF_PLUGIN_URL . '/css/thickbox.css');
		wp_enqueue_style('sraf_styles');
		wp_enqueue_style('thick_box_styles');
	}
	
	/**
	 * Add necessary stylesheet to the head.
	 */
	function addAdminStyleSheets() {
		$this->addStyleSheets();
	}

	/**
	 * Add necessary scripts to the head.
	 */
	function addScripts() {
		wp_register_script('thick_box_script', SRAF_PLUGIN_URL . '/js/thickbox.js');
		wp_register_script('jquery_validator', SRAF_PLUGIN_URL . '/js/jquery.validate.js');
		wp_register_script('sraf_scripts', SRAF_PLUGIN_URL . '/js/sraf.js');
		
		wp_enqueue_script('jquery');
		wp_enqueue_script('jquery_validator');
		wp_enqueue_script('thick_box_script');
		wp_enqueue_script('sraf_scripts');
		
		// Ticket #4. When theme doesn't have "wp_footer", sponsor link might
		// not be added. Let us do it in hard way :(
		print(
			'<script type="text/JavaScript">' . "\n" .
				'var srafSponsorWrapperClass = ' . json_encode($this->getSponsorLinkWrapperClass()) . ';' . "\n" .
				'var srafSponsorText = ' . json_encode($this->getSponsorLink()) . ';' . "\n" .
			'</script>' . "\n"
		);
	}
	
	/**
	 * Add necessary scripts to the head.
	 */
	function addAdminScripts() {
		$this->addScripts();
	}
	
	function addSponsorLinks() {
		echo $this->getSponsorLink();
	}
	
	function getSponsorLink() {
		if(get_option('sraf_linkback') != '1') {
			return('<div class="' . $this->getSponsorLinkWrapperClass() . '">' . SRAF_SPONSOR_LINK . '</div>');
		}
	}
	
	function getSponsorLinkWrapperClass() {
		return 'sraf-sponsor-link-wrapper';
	}
	
	function getMaxCaptchaCharNum() {
		return get_option(SRAF_WIDGET_CAPTCHA_CHAR_NUM_OPT);
	}
	
	function getMaxMsgBodyLen() {
		return get_option(SRAF_WIDGET_MSG_BODY_MAX_LEN_OPT);
	}
	
	function getMaxNumOfEmail() {
		return get_option(SRAF_WIDGET_MAX_NUM_OF_EMAIL_OPT);
	}
	
	function getEmailSendInterval() {
		return get_option(SRAF_WIDGET_EMAIL_SEND_INTERVAL_OPT, SRAF_DEFAULT_EMAIL_SEND_INTERVAL );
	}

	function setDefaultOptionValue($optionName, $defaultValue) {
		if (!get_option($optionName)){
			add_option($optionName, $defaultValue);
		}
	}

	function updatePostedOptions($namePrefix, $controlOptions) {
		foreach ( $controlOptions as $optionName => $controlOption ) {
			$this->updatePostedOptionIfFound(
				$namePrefix . '_widget_' . $controlOption['#name'],
				$optionName,
				isset($controlOption['#rule']) ? $controlOption['#rule'] : array()
			);
		}
	}
	
	/**
	 * $rule is as defined in validator.
	 */
	function updatePostedOptionIfFound($postedKey, $optionName, $rule = array()) {
		if (!isset($_POST[$postedKey])) {
			return;
		}
		
		$value = $_POST[$postedKey];
		$errorMsg = $this->validator->validate($value, $rule);
		if ($errorMsg) {
			if (!isset($rule['default-value'])) {
				return;
			}
			$value = $rule['default-value'];
		}
		update_option($optionName, $value);
	}

	function updatePostedOptionWithOtherIfFound($postedKey, $optionName) {
		if (isset($_POST[$postedKey]) && $_POST[$postedKey] != 'other') {
			update_option($optionName, $_POST[$postedKey]);
		} elseif(isset($_POST[$postedKey . '-other'])) {
			update_option($optionName, $_POST[$postedKey . '-other']);
		}
	}
	
	function getOptionsForm($namePrefix, $controlOptions) {
		$htmlAttrNames = array('class', 'size', 'rows', 'cols', 'help_text', 'prefix', 'suffix');
		$str = '';
		
		foreach ( $controlOptions as $optionName => $controlOption ) {
			$title = __($controlOption['#title']) . ': ';
			$postedKey = $namePrefix . '_widget_' . $controlOption['#name'];
			
			$renderOptions = array();
			foreach ($htmlAttrNames as $htmlAttrName) {
				if (isset($controlOption['#' . $htmlAttrName])) {
					$renderOptions[$htmlAttrName] = $controlOption['#' . $htmlAttrName];
				}
			} 
			
			if ($controlOption['#type'] == 'text') {
				$str .= $this->getOptionFormText($title, $postedKey, $optionName, $renderOptions);
			}
			else if ($controlOption['#type'] == 'textarea') {
				$str .= $this->getOptionFormTextArea($title, $postedKey, $optionName, $renderOptions);
			}
		}
		
		return $str;
	}

	function getOptionFormText($label, $postedKey, $optionName, $options = array()) {
		$options['wrapper_class'] = 'option-form-item option-form-text ' . $postedKey;
		return $this->getFormText($label, $postedKey, get_option($optionName), $options);
	}

	function getOptionFormTextArea($label, $postedKey, $optionName, $options = array()) {
		return $this->getFormTextArea(
			$label, $postedKey, get_option($optionName), 'option-form-item option-form-textarea ' . $postedKey, $options
		);
	}

	function getOptionFormSelectWithOther($label, $postedKey, $selectOptions, $optionName) {
		return $this->getFormSelectWithOther(
			$label, $postedKey, $selectOptions, get_option($optionName), 'option-form-item option-form-select-with-other ' . $postedKey
		);
	}

	function getFormText($label, $name, $value, $options = array()) {
		$wrapperClass = '';
		if (isset($options['wrapper_class'])) {
			$wrapperClass = $options['wrapper_class'];
			unset($options['wrapper_class']);
		}
		
		$helpText = '';
		if (isset($options['help_text'])) {
			$helpText = $options['help_text'];
			unset($options['help_text']);
		}
		
		$prefix = '';
		if (isset($options['prefix'])) {
			$prefix = $options['prefix'];
			unset($options['prefix']);
		}
		
		$suffix = '';
		if (isset($options['suffix'])) {
			$suffix = $options['suffix'];
			unset($options['suffix']);
		}
		
		return (
			'<div class="form-item form-item-textfield ' . $wrapperClass . '">' .
				(!$prefix ? '' : $prefix) .
				(!$label ? '' : '<label for="' . $name . '">' . $label . '</label>') .
				'<input id="' . $name . '" type="text" name="' . $name . '" value="' . $value .  '" ' . $this->transformArrayIntoHtmlAttrs($options) . ' />' .
				(!$helpText ? '' : '<span class="mimp-form-help-text">' . $helpText . '</span>') .
				(!$suffix ? '' : $suffix) .
			'</div>'
		);
	}
	
	function getFormTextArea($label, $name, $value, $wrapperClass = '', $options = array(), $helpText = '') {
		return (
			'<div class="form-item form-item-textarea ' . $wrapperClass . '">' .
				(!$label ? '' : '<label for="' . $name . '">' . $label . '</label>') .
				'<textarea name="' . $name . '" ' . $this->transformArrayIntoHtmlAttrs($options) . '>' . $value .  '</textarea>' .
				(!$helpText ? '' : '<span class="mimp-form-help-text">' . $helpText . '</span>') .
			'</div>'
		);
	}

	function getFormCheckbox($label, $name, $value, $checked = false, $wrapperClass = '', $helpText) {
		return (
			'<div class="form-item form-item-checkbox ' . $wrapperClass . '">' .
			'	<label for="' . $name . '">' . $label . '</label>' .
			'	<input type="checkbox" name="' . $name . '" value="' . $value .  '" ' . ($checked ? 'checked="checked"' : '') . ' />' .
			'	<span class="mimp-form-help-text">' . $helpText . '</span>' .
			'</div>'
		);
	}

	function getFormSelect($label, $name, $options, $selected = '', $wrapperClass = '') {
		$optionList = '';
		$optionList .= '<option value="">&lt;Select&gt;</option>';
		foreach($options as $key => $value) {
			$optionList .= '<option value="' . $key . '" ' . ($key == $selected ? 'selected = selected' : '') . '>' . $value . '</option>';
		}
		return (
			'<div class="form-item form-item-select ' . $wrapperClass . '">' .
			'	<label for="' . $name . '">' . $label . '</label>' .
			'	<select name="' . $name . '">' . $optionList . '</select>'.
			'</div>'
		);
	}

	function getFormSelectWithOther($label, $name, $options, $selected = '', $selectWrapperClass = '') {
		$otherTextfieldDefVal = '';
		if ($selected != '' && !in_array($selected, $options)){
			$otherTextfieldDefVal = $selected;
			$selected = 'other';
		}
		$options['other'] = 'Other';
		
		$formOptions = array(
			'wrapper_class' => 'other-option-textfield'
		);
		return (
			'<div class="mimp-select-box-with-other-option">' .
			$this->getFormSelect($label, $name, $options, $selected, $selectWrapperClass) .
			$this->getFormText('Other', $name . '-other', $otherTextfieldDefVal, $formOptions) .
			'<div class="clear"></div></div>'
		);
	}

	function getFormMarkup($value = '') {
		return ($value);
	}

	function getPostedText($key, $defaultValue = '') {
		return isset($_POST[$key]) ? $_POST[$key] : $defaultValue;
	}

	function getPostedCheckbox($key, $defaultValue = '0') {
		return (isset($_POST[$key]) && $_POST[$key] != '') ? '1' : $defaultValue;
	}

	function getPostedSelect($key, $defaultValue = '') {
		return (isset($_POST[$key])) ? $_POST[$key] : $defaultValue;
	}

	function getPostedSelectWithOther($key, $defaultValue = '') {
		if (isset($_POST[$key]) && $_POST[$key] != '' && $_POST[$key] != 'other'){
			return $_POST[$key];
		}
		if (isset($_POST[$key . '-other'])){
			return $_POST[$key . '-other'];
		}
		return $defaultValue;
	}

	function getPostedTextValidHtml($key, $defaultValue = '') {
		$supportedTags = array(
			'a' => array(), 'b' => array(), 'i' => array(),
			'br' => array(), 'em' => array(), 'strong' => array(),
			'span' => array(), 'div' => array(),
		);
		return wp_kses($this->getPostedText($key, $defaultValue), $supportedTags);
	}

	function getStatusMessage($msg, $cssClass = '') {
		return '<div class="mimp-status-message ' . $cssClass . '">' . $msg . '</div>';
	}

	function getErrorMessage($msg, $cssClass = '') {
		return '<div class="mimp-error-message ' . $cssClass . '">' . $msg . '</div>';
	}

	/**
	 * Transform the array $arr into a string formatted by $format and separated by
	 * $itemSeparator. For example,
	 * 1)
	 *    $arr = array("abc" => "def", "ghi" => "jkl");
	 *    $itemSeparator = ",";
	 *    $format = '%k% = "%v%"';
	 *    Output = 'abc = "def",ghi = "jkl"'
	 * 
	 * 2)
	 *    $arr = array("abc" => "def", "ghi" => "jkl");
	 *    $itemSeparator = ", ";
	 *    $format = '%v%';
	 *    Output = 'def, jkl'
	 * 
	 * @param Any[] $arr - Array to transform. Mandatory.
	 * @param String $format - Formattor to format the output. Optional.
	 * @param String $itemSeparator - Separator to use in the string repsentation. Optional.
	 * 
	 * @return String - Constructed string.
	 */
	function transformArrayIntoString($arr, $format = '%k%: %v%', $itemSeparator = ', ') {
		if (!$arr || count($arr) <= 0) {
			return('');
		}
		$finalStr = '';
		foreach ( $arr as $key => $value ) {
			if (strlen($finalStr) > 0) {
				$finalStr .= $itemSeparator;
			}
			$finalStr .= str_replace('%v%', $value, str_replace('%k%', $key, $format));
		}
		
		return($finalStr);
	}
	
	function transformArrayIntoHtmlAttrs($attributes) {
		return $this->transformArrayIntoString($attributes, '%k%="%v%"', ' ');
	}
	
	function escapeHtmlEntities($myHtml){
		return htmlentities($myHtml, ENT_NOQUOTES);
	}

	/**
	 *	Get the name of the IP table name
	 *
	 *	@return the IP table name
	 */
	function getIpTableName() {
		global $wpdb;
		return $wpdb->prefix . 'sraf_ip';
	}
	
	function checkEmailCanBeSentNow($ip = null){
		if (!$ip) {
			return 'IP address is not valid';
		}
		
		date_default_timezone_set("UTC");
		
		$fromTime = $this->_getLastSentDateFromIP($ip);
		if (empty($fromTime[0])) {
			return '';
		}
		
		$max_allowable_time = $this->getEmailSendInterval() * 60; //Make it seconds
		$difference_time = strtotime(date("Y-m-d H:i:s")) - strtotime($fromTime[0]);
		if ($difference_time < $max_allowable_time) {
			return 'Please wait at least '. (ceil($max_allowable_time / 60) - ceil( $difference_time / 60 )). ' minutes to send next email.';
		}
		
		return '';
	}
	
	function registerSentEmailIP(){
		date_default_timezone_set('UTC');
		
		$requestIP = $_SERVER['REMOTE_ADDR'];
		$fromTime = $this->_getLastSentDateFromIP($requestIP);
		
		global $wpdb;
		if (!empty($fromTime[0])){
			$wpdb->update($this->getIpTableName(), array( 'date' => date("Y-m-d H:i:s") ), array( 'ip' => $requestIP ), array( '%s' ), array( '%s' ) );
		} else {
			$wpdb->insert($this->getIpTableName(), array( 'date' => date("Y-m-d H:i:s") ), array( '%s' ) );
		}
	}
	
	function _getLastSentDateFromIP($ip) {
		global $wpdb;
		return $wpdb->get_col('SELECT date FROM ' . $this->getIpTableName() . ' WHERE ip = "' . $ip . '"');
	}	
}
