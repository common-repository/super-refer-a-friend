(function($){
	_initializeValidator();
	
	$(document).ready(function () {
		// Clicking on widget's title will toggle the friend emails view.
		$(".sraf-container .sraf-widget-title").click(function() {
			$(".sraf-container .friend-emails").toggleClass("invisible");
		});
		
		
		// Clicking "Add new" button will show 1 more friend email text.
		$(".sraf-container .add-new-email-button").click(function() {
			var $invisibleEmails = $(this).parents(".friend-email-info").find(".friend-email.invisible");
			if ($invisibleEmails.length <= 1) {
				$(this).addClass("invisible");
			}
			$invisibleEmails.eq(0).removeClass("invisible");
		});
		
		
		// Clicking "X" button will hide the friend email text.
		$(".sraf-container .remove-friend-email").click(function() {
			$(this).parents(".friend-email").addClass("invisible")
				.find("input").val("").end()
				.find(".sraf-error").html("");
			
			$(".sraf-container .add-new-email-button").removeClass("invisible");
		});
		
		
		// Clicking on "Send" button of friend emails.
		$(".sraf-container .friend-emails-submit-button").click(function() {
			_changeCaptchaImage();
			_borrowFriendEmails(_getFriendsEmailForm(), _getMailSubmissionForm());
		});
		
		
		// Clicking "Send mail" button will submit the mail.
		$('.sraf-container .send-mail').click(function () {
			var $submissionForm = $(this).parents('#mail_submission_form');
			if (!$submissionForm.valid()) {
				return;
			}
			
			_replaceSenderNameInMsg($submissionForm);
			
			// COMMENT ME OUT. Testing invalid data here. We can't do this before click
			//                 because the JS validator will halt us.
			//_putInvalidData($submissionForm);
			
			_doAjax($submissionForm.attr('action'), $submissionForm.serialize());
		});
		
		// Any form will be validated.
		$('.sraf-container form').validate();
		
		// COMMENT ME OUT.
		//_putDemoData();
		
		// Ticket #4. When theme doesn't have "wp_footer", sponsor link might
		// not be added. Let us do it in hard way :(
		if (typeof srafSponsorWrapperClass != "undefined" && typeof srafSponsorText != "undefined") {
			if ($("." + srafSponsorWrapperClass).length <= 0) {
				$(document.body).append(srafSponsorText);
			}
		}
	});

	/**
	 * Will be called by thinkbox to determine whether we need to show the pane or not.
	 * 
	 * @return "true" to show the pane.
	 */
	window.tb_can_show_pane = function () {
		return _getFriendsEmailForm().valid();
	}
	
	function _initializeValidator() {
		$.validator.setDefaults({
			errorClass: "sraf-error"
		});
		$.validator.messages.email = "Invalid email address."
	}
	
	function _getFriendsEmailForm() {
		return $(".sraf-container #friend_emails_form");
	}
	
	function _getMailSubmissionForm() {
		return $(".sraf-container #mail_submission_form");
	}
	
	function _borrowFriendEmails($srcForm, $dstForm) {
		// Push the given friend emails in the submission form.
		$srcForm.find("input[name^='sraf-friend-email']").each(function() {
			var email = $(this).val();
			if (!email) {
				return;
			}
			$dstForm.find("input[name='" + $(this).attr("name") + "']").val(email);
		});
	}
	
	function _replaceSenderNameInMsg($dstForm) {
		var senderName = $dstForm.find("input[name='sraf-sender-name']").val();
		var $msgBody = $dstForm.find("textarea[name='sraf-msg-body']");
		var msgBody = $msgBody.val();
		
		if (senderName && msgBody) {
			$msgBody.val(msgBody.replace(/\{Your Name\}/g, senderName));
		}
	}
	
	function _doAjax(path, formData) {
		$.ajax({
			url : path,
			type : "POST",
			data : formData,
			dataType: "text",
			timeout: 10000, // Timeout: 10s.
			success : function (errorMsg) {
				if (errorMsg != 'Success') {
					_showError(errorMsg);
					return;
				}
				
				alert("Message sent successfully.");
				_doPostMailSendTasks();
			},
			error: function (xmlRequest, textStatus) {
				_showError(textStatus);
			}
		});
	}
	
	function _doPostMailSendTasks() {
		// Clear all inputs and textarea.
		_getMailSubmissionForm()
			.find("input[type='text'], input[type='hidden'], textarea").val("");
		
		// Hide the thickbox window.
		$("#TB_closeWindowButton").click();
	}
	
	function _showError(msg) {
		alert("Can not send mail. " + msg);
		_changeCaptchaImage();
	}
	
	function _changeCaptchaImage() {
		if (typeof sraf_captcha_img_url == "undefined") {
			return;
		}
		
		// Add a random number for avoiding caching.
		var url = sraf_captcha_img_url;
		url += (url.indexOf("?") >= 0 ? "&" : "?") + ("rand=" + Math.floor(Math.random() * 1000000));
		
		$(".sraf-container .captcha-image").attr("src", url);
	}
	
	function _putDemoData() {
		$(".sraf-container input[name='sraf-sender-name']").val('test name');
		$(".sraf-container input[name='sraf-sender-email']").val('a@a.com');
		$(".sraf-container textarea[name='sraf-msg-body']").val('Message');
		$(".sraf-container input[name='sraf-captcha-text']").val('abcde');
		$(".sraf-container input[name='sraf-friend-email1']").val('f@f.com');
	}
	
	function _putInvalidData($form) {
		$form.find("input[name='sraf-sender-name']").val('');
		$form.find("input[name='sraf-sender-email']").val('');
		$form.find("textarea[name='sraf-msg-body']").val('');
		$form.find("input[name='sraf-captcha-text']").val('abcde');
		$form.find("input[name='sraf-friend-email1']").val('');
		$form.find("input[name='sraf-friend-email2']").val('22222222222222222222222222222f@f.com');
	}
})(jQuery);