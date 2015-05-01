var resizeHandler = function() {
	console.log("resize handler running");
	var height = $(document).height();
	var nbh = $('nav').height();
	$('#fullcontainer').height(height-nbh);
}

serverAddress = "localhost:5000"

$(document).ready(function() {
	resizeHandler();
	$('#configModalTrigger').on('click', function() {
		console.log("click configModal")
		$('#configModal').openModal();
		$('#addrinput').val(serverAddress);
	});
	$('#addrinput').on('keydown', function(ev) {
		if (ev.keyCode != 13) {
			return;
		}
		serverAddress = $('#addrinput').val();
		$('#configModal').closeModal();
	});
	$('#saveAddr').on('click', function() {
		serverAddress = $('#addrinput').val();
		$('#configModal').closeModal();
	});


	$('#executeButton').on('click', function() {
		$('#execModal').openModal();
		$('#execStatus').text('Running on server ' + serverAddress + "...");
		$('#return').empty();
		$.ajax({
			url: "http://" + serverAddress + "/exec",
			type: "POST",
			data: JSON.stringify({program: $('#editor').val().split('\n')}),
			contentType: "application/json",
  			dataType: "json",
  			success: function(data) {
  				$('#execStatus').text("Execution Results:");
  				for (var c = 0; c < data.length; c++) {
  					$('#return').append("<div class=\"monospace\">" + data[c] + "</div>");
  				}
  			},
  			error: function() {
  				$('#execStatus').text("Failed to execute code.");
  			}
		});
	});
});

$(window).resize(resizeHandler);

