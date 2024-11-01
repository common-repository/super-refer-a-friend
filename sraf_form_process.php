<?php
	session_start();
	
	require_once('../../../wp-load.php');
	require_once('includes/sraf-config.inc');
	require_once('includes/sraf-validator.inc');
	require_once('includes/sraf-common.inc');
	
	
	
	// REMEMBER: Global variables declared here are not visible within function.
	
	function sraf_send_posted_mail($sraf_common) {
		$errorMsgs = array();
		
		$fromName = sraf_validate_sender_name($errorMsgs, $sraf_common);
		$fromEmail = sraf_validate_sender_email($errorMsgs, $sraf_common);
		$bodyMsg = sraf_validate_message_body($errorMsgs, $sraf_common);
		$toEmails = sraf_validate_to_emails($errorMsgs, $sraf_common);
		
		if (count($errorMsgs) > 0) {
			return "Errors:\n\t-  " . implode("\n\t-  ", $errorMsgs);
		}
		
		$headers = (
			"MIME-Version: 1.0" . "\r\n" .
			"Content-type:text/html;charset=iso-8859-1" . "\r\n" .
			"From: $fromName <$fromEmail>"
		);
		
		$toMessage = (
			"A friend of yours has sent you a link from ". get_bloginfo('name') . "\r\n" .
			"His / her message was:\r\n" . $bodyMsg
		);
		//echo $toMessage;
		$failedEmails = array();
		foreach($toEmails as $toEmail){
			if (!@wp_mail($toEmail, "Referred Blog Link", $toMessage, $headers)) {
				$failedEmails[] = $toEmail;
			}
		}
		if (count($failedEmails) > 0) {
			return "There was a problem sending emails to the following addresses:\n\t-" . implode("\n\t-", $failedEmails);
		}
		
		return NULL;
	}
	
	function sraf_is_posted_captcha_matched() {
		if (empty($_SESSION['sraf-captcha-code'])) {
			return FALSE;
		}
		
		$value = trim($_POST['sraf-captcha-text']);
		return $_SESSION['sraf-captcha-code'] == mysql_real_escape_string($value);
	}
	
	function sraf_validate_sender_name(&$errorMsgs, $sraf_common) {
		$senderName = trim($_POST['sraf-sender-name']);
		
		$error = $sraf_common->getValidator()->validate($senderName, array(
			'type' => 'string', 'max' => SRAF_MAX_NAME_LENGTH,
			'min-error-msg' => 'No sender name provided',
			'max-error-msg' => 'Sender name Too Long',
		));
		if ($error) {
			$errorMsgs[] = $error;
		}
		
		return $sraf_common->escapeHtmlEntities($senderName);
	}
	
	function sraf_validate_sender_email(&$errorMsgs, $sraf_common) {
		$fromEmail = trim($_POST['sraf-sender-email']);
		
		$error = $sraf_common->getValidator()->validate($fromEmail, array(
			'type' => 'email', 'max' => SRAF_MAX_EMAIL_LENGTH,
			'min-error-msg' => 'No sender email provided',
			'max-error-msg' => 'Sender name Too Long',
			'error-msg' => 'Invalid sender email',
		));
		if ($error) {
			$errorMsgs[] = $error;
		}
		
		return $fromEmail;
	}
	
	function sraf_validate_message_body(&$errorMsgs, $sraf_common) {
		$bodyMsg = trim($_POST['sraf-msg-body']);
		
		$error = $sraf_common->getValidator()->validate($bodyMsg, array(
			'type' => 'string', 'max' => $sraf_common->getMaxMsgBodyLen(),
			'min-error-msg' => 'No message provided',
			'max-error-msg' => 'Message Too Long',
		));
		if ($error) {
			$errorMsgs[] = $error;
		}
		
		return $sraf_common->escapeHtmlEntities($bodyMsg);
	}
	
	function sraf_validate_to_emails(&$errorMsgs, $sraf_common) {
		$toEmails = sraf_get_posted_friend_emails($sraf_common);
		if (!$toEmails) {
			$toEmails = array();
		}
		
		$invalidEmailErrors = array();
		
		// Validate all the recepient emails. Skip empty emails.
		foreach ( $toEmails as $key => $toEmail) {
			if (!$toEmail) {
				unset($toEmails[$key]);
				continue;
			}
			$errorMsg = $sraf_common->getValidator()->validate($toEmail, array(
				'type' => 'email', 'max' => SRAF_MAX_EMAIL_LENGTH,
			));
			if ($errorMsg) {
				$invalidEmailErrors[] = $toEmail . ' [ ' . $errorMsg . ' ]';
			}
		}
		
		if (count($invalidEmailErrors) > 0) {
			$errorMsgs[] = "Erroneous email addresses:\n\t\t-  " . implode("\n\t\t-  ", $invalidEmailErrors);
		}
		
		if (count($toEmails) <= 0) {
			$errorMsgs[] = 'No recipient email address provided';
		}
		
		return $toEmails;
	}
	
	function sraf_get_posted_friend_emails($sraf_common) {
		$toEmails = array();
		
		$maxFriends = $sraf_common->getMaxNumOfEmail();
		for ($i = 1; $i <= $maxFriends; $i++){
			$postedKey = 'sraf-friend-email' . $i;
			if (isset($_POST[$postedKey]) && strlen(trim($_POST[$postedKey]))){
				$toEmails[] = $_POST[$postedKey];
			}
		}
		return $toEmails;
	}
	
	function sraf_process_posted_mail_request() {
		if (!sraf_is_posted_captcha_matched()) {
			die('Security code did not match.');
			return;
		}
		
		$sraf_common = new SRAFCommon();
		
		$errorMsg = $sraf_common->checkEmailCanBeSentNow($_SERVER['REMOTE_ADDR']);
		if ($errorMsg && $errorMsg != '') {
			die($errorMsg);
			return;
		}
		
		// FIXME: Send JSON?
		$errorMsg = sraf_send_posted_mail($sraf_common);
		if ($errorMsg && $errorMsg != '') {
			die($errorMsg);
			return;
		}
		
		$sraf_common->registerSentEmailIP();
		die('Success');
	}
	

	// Boot.
	sraf_process_posted_mail_request();
