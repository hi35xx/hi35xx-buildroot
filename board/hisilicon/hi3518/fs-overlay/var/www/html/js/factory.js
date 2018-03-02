window.onload = function() {
  xmlHttp = null;
  if (window.XMLHttpRequest) {
    xmlHttp = new XMLHttpRequest();
  }
  else if (window.ActiveXObject) {
    xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
  }
  if (xmlHttp != null) {
    xmlHttp.onreadystatechange = function() {
      if (xmlHttp.readyState == 4) {
        if (xmlHttp.status == 200) {
          var json = JSON.parse(xmlHttp.responseText);
          document.getElementById("deviceid").value = json.system.deviceid;
          document.getElementById("securecode").value = json.system.securecode;
        }
        else {
          alert("Failed to retrive factory data");
        }
      }
    }
    xmlHttp.open("GET", "/api/factory.json", true);
    xmlHttp.send(null);
  }
};

function saveconfig() {
  xmlHttp = null;
  if (window.XMLHttpRequest) {
    xmlHttp = new XMLHttpRequest();
  }
  else if (window.ActiveXObject) {
    xmlHttp = new ActiveXObject("Microsoft.XMLHTTP");
  }
  if (xmlHttp != null) {
    xmlHttp.onreadystatechange = function() {
      if (xmlHttp.readyState == 4) {
        if (xmlHttp.status == 200) {
          alert("OK");
        }
        else {
          alert("Failed to save factory data");
        }
      }
    }
    xmlHttp.open("POST", "/api/factory.json", true);
    var data = JSON.stringify({
      "system" : {
        "deviceid" : document.getElementById("deviceid").value,
        "securecode" : document.getElementById("securecode").value
      }
    });
    xmlHttp.send(data);
  }
}
