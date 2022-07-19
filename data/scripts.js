document.getElementById("toggle_led").addEventListener('click', toggle_led);
document.getElementById("brightness_slider").addEventListener('click', set_brightness);
document.getElementById("colorpicker").addEventListener('input', set_color);
document.getElementById("specific_color_select").addEventListener('input', set_specific_color);
document.getElementById("effect_select").addEventListener('input', set_effect);
document.addEventListener("DOMContentLoaded", get_settings);

function get_settings(){
    var request = new XMLHttpRequest();
    request.open('GET', '/get_settings', true);
    request.responseType = 'json';
    request.onload = function(){
        if(request.readyState == 4 && request.status == 200){
            var jsonObj = request.response;
            document.getElementById("toggle_led").checked = jsonObj['led_state'];
            document.getElementById("brightness_slider").value = jsonObj['brightness'];
            document.getElementById("colorpicker").value = jsonObj['color'];
            document.getElementById("specific_color_select").value = jsonObj['specific_color'];
            document.getElementById("effect_select").value = jsonObj['effect'];
        }
    }
    request.send();
}

function toggle_led(){
    var led_state = document.getElementById("toggle_led").checked;
    var request = new XMLHttpRequest();
    request.open('GET', '/toggle_led?state=' + led_state, false);
    request.send();
    console.log(led_state);
}

function set_brightness(){ 
    var brightness = document.getElementById("brightness_slider").value;
    var request = new XMLHttpRequest();
    request.open('GET', '/set_brightness?brightness=' + brightness, false);
    request.send();
    console.log(brightness);
}

function set_color(){
    var color = document.getElementById("colorpicker").value.slice(1);
    var request = new XMLHttpRequest();
    request.open('GET', '/set_color?color=' + color, false);
    request.send();
    console.log(color);
}

function set_specific_color(){
    var specific_color = document.getElementById("specific_color_select").value;
    var request = new XMLHttpRequest();
    request.open('GET', '/set_specific_color?specific_color=' + specific_color, false);
    request.send();
    console.log(specific_color);
}

function set_effect(){
    var effect = document.getElementById("effect_select").value;
    var request = new XMLHttpRequest();
    request.open('GET', '/set_effect?effect=' + effect, false);
    request.send();
    console.log(effect);
}