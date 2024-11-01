<?php
class SRAFValidator {
	/**
	 * $rule = array(
	 *    'type' => 'number',
	 *    'min' => 10,
	 *    'max' => 1000,
	 * )
	 * 
	 * $rule = array(
	 *    'type' => 'string',
	 *    'min' => 10,
	 *    'max' => 1000,
	 * )
	 */
	function validate($value, $rule = array()) {
		if (!isset($rule) || !isset($rule['type']) || !in_array($rule['type'], $this->_supportedTypes())) {
			return NULL;
		}
		
		if ($rule['type'] == 'string') {
			return $this->_validateString($value, $rule);
		}
		
		if ($rule['type'] == 'integer') {
			return $this->_validateInteger($value, $rule);
		}
		
		if ($rule['type'] == 'float') {
			return $this->_validateFloat($value, $rule);
		}
		
		if ($rule['type'] == 'email') {
			return $this->_validateEmail($value, $rule);
		}
		
		return NULL;
	}
	
	function _validateString($value, &$rule) {
		if (!is_string($value)) {
			return 'Invalid string';
		}
		
		$min = isset($rule['min']) ? intval($rule['min']) : 1;
		$max = isset($rule['max']) ? intval($rule['max']) : 1000;
		$len = strlen($value);
		
		if ($len > $max) {
			return isset($rule['max-error-msg']) ? $rule['max-error-msg'] : 'Too long value';
		}
		if ($len < $min) {
			return isset($rule['min-error-msg']) ? $rule['min-error-msg'] : 'Too short value';
		}
		
		return NULL;
	}
	
	function _validateInteger($value, &$rule) {
		if (!is_numeric($value)) {
			return 'Invalid number';
		}
		
		$min = isset($rule['min']) ? intval($rule['min']) : 0;
		$max = isset($rule['max']) ? intval($rule['max']) : 100000000;
		
		if (strlen($value) > 9 || intval($value) > $max) {
			return isset($rule['max-error-msg']) ? $rule['max-error-msg'] : 'Integer above maximum range';
		}
		if (intval($value) < $min) {
			return isset($rule['min-error-msg']) ? $rule['min-error-msg'] : 'Integer below minimum range';
		}
		
		return NULL;
	}
	
	function _validateFloat($value, &$rule) {
		if (!is_numeric($value)) {
			return 'Invalid number';
		}
		
		$min = isset($rule['min']) ? floatval($rule['min']) : 0.0;
		$max = isset($rule['max']) ? floatval($rule['max']) : 100000000.0;
		
		if (strlen($value) > 9 || floatval($value) > $max) {
			return isset($rule['max-error-msg']) ? $rule['max-error-msg'] : 'Number above maximum range';
		}
		if (floatval($value) < $min) {
			return isset($rule['min-error-msg']) ? $rule['min-error-msg'] : 'Number below minimum range';
		}
		
		return NULL;
	}
	
	function _validateEmail($value, &$rule, $checkDomain = false) {
		$errorMsg = $this->_validateString($value, $rule);
		if ($errorMsg) {
			return $errorMsg;
		}
		
		$errorMsg = isset($rule['error-msg']) ? $rule['error-msg'] : 'Invalid email';
		if (!ereg(
			'^[-!#$%&\'*+\\./0-9=?A-Z^_`a-z{|}~]+'.'@'.
			'[-!#$%&\'*+\\/0-9=?A-Z^_`a-z{|}~]+\.'.
			'[-!#$%&\'*+\\./0-9=?A-Z^_`a-z{|}~]+$', $value
		)) {
			return $errorMsg;
		}
		
		if (!$checkDomain || !function_exists('checkdnsrr')) {
			return NULL;
		}
		
		list (, $domain)  = explode('@', $value);
		if (checkdnsrr($domain.'.', 'MX') || checkdnsrr($domain . '.', 'A')) {
			return NULL;
		}
		
		return $errorMsg;
	}
	
	function _supportedTypes() {
		return array('string', 'integer', 'float', 'email', 'escapehtml');
	}
}