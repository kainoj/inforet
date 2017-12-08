$(document).ready(function() {
  var host = window.location.hostname;
  var port = window.location.port;
  var api  = "http://" + host + ":" + port + "/api";
  console.log("API location: " + api);

  // Handle keyup on search input
  $("#input").keyup(function() {
    var query = $("#input").val();
    var url   = api + "?q=" + query;

    $.getJSON(url, function(response) {
      console.log("Got response for query: " + query);

      var html = "";
      for (var i = 0; i < response.res.length; i++) {
        html += "<b>" + response.res[i].name + "</b> - ";
        html += "<i>" + response.res[i].description + "</i>";
        html += "</br>"
      }

      $("#results").html(html);
      $("#number").html("Found: " + response.found);
    })
  })

});


// // Translates JSON to HTML
// function jsonToHtml(json) {
//   var html = "";
//
//
//     html += "<h4>" + e.res.name + "</h4>";
//     html += "<i>" + e.res.description + "</i>";
//     html += "</br>"
//   }
// }
