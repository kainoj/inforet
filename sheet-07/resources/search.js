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
      if ( $("#input").val() != "" ) {
        $("#qry").html("\"" + esc(query) + "\": ")
        $("#results").html(jsonToHtml(response.res));
        $("#number").html("found: " + response.found);
      }
      else {
        console.log("EMPTY");
        // That means that input field is already empty -> nothing to show
        $("#result").html("");
        $("#number").html("");
        $("#qry").html("");
      }
    })
  })

  coolCoookiesUsage();
});


// Translates JSON to HTML
function jsonToHtml(entities) {
  var html = "";
  for (var e in entities) {
    html += "<div class=\"entity\">";
    // Result + URL
    html += "<div class=\"result\">";
    html += "<a href=\"" + esc(entities[e].wikipediaUrl) + "\">";
    html += esc(entities[e].name) + "</a></div>";
    // Description
    html += "<div class=\"description\">";
    html += esc(entities[e].description);
    html += "</div></div>"
  }
  return html
}

// Escapes special HTML characters
function esc(html) {
  if (html == "") return "";
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
