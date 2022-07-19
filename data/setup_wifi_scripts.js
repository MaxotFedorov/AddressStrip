document.getElementById("confirm").addEventListener('click', save_settings);

function save_settings(){
    var ssid = document.getElementById("ssid").value;
    var pass = document.getElementById("pass").value;
    var request = new XMLHttpRequest();
    request.open('GET', '/save_wifi_settings?ssid=' + ssid + "&pass=" + pass, false);
    request.send();
    console.log(ssid);
    console.log(pass);
}