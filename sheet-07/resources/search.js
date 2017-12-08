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
      $("#results").html(jsonToHtml(response.res));
      $("#number").html("Found: " + response.found);
    })
  })

  coolCoookiesUsage();
});


// Translates JSON to HTML
function jsonToHtml(entities) {
  var html = "";
  for (var e in entities) {
    html += "<b>" + esc(entities[e].name) + "</b> - ";
    html +=  esc(entities[e].description);
    html += "</br>"
  }
  return html
}

// Escapes special HTML characters
function esc(html) {
  return html.replace(/</g, "&lt;").replace(/>/g, "&gt;");
}

// Cool cookies usage
function coolCoookiesUsage() {
  var lastVisitDate = Cookies.get("lastVisitDate");
  var counter = Cookies.get('counter');

  if (lastVisitDate) {
    $("#visit").html("Your last visit on this page was on " + lastVisitDate);
    Cookies.set('lastVisitDate', new Date());
  }
  else {
    Cookies.set('lastVisitDate', new Date());
  }

  if (counter) {
    $("#counter").html("You visited this site " + counter + " times.");
    Cookies.set('counter', parseInt(counter)+1);
    console.log("sad");
  }
  else {
    Cookies.set('counter', 0);
  }
}
