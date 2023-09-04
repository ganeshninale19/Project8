
var xhr = new XMLHttpRequest();
var devices = ["geyser","light","AC", "fan"];

var count = 0;

setInterval(fetch, 1000);

function fetch(){

    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
            let response= JSON.parse(xhr.response);
            var statusElement = document.getElementById(response[1] + 'Status');
            var buttonElement = document.getElementById(response[1] + 'Button');


            if(response[2]===1){
                statusElement.textContent = 'On';
                statusElement.classList.remove('status-off');
                statusElement.classList.add('status-on');
                buttonElement.textContent = 'Turn Off';
            }
            else {
                statusElement.textContent = 'Off';
                statusElement.classList.remove('status-on');
                statusElement.classList.add('status-off');
                buttonElement.textContent = 'Turn On';
            }
            
             if(buttonElement.hasAttribute('hasgauge')){
                var GaugeNeedle = document.getElementById(response[1] + 'Guage');
                let angle = (response[4]/1000)*270;
                GaugeNeedle.setAttribute("style",`transform:rotate(${angle}deg);`);
            }
            
        }
    };
    
    
    var get = `?appliance=${devices[count]}`;
    count = (count+1)%3;
    xhr.open('GET', '/fetch'+get, true);
    
    xhr.send();
}

function toggleDevice(device){

var statusElement = document.getElementById(device + 'Status');
var buttonElement = document.getElementById(device + 'Button');

var On = `appliance=${device}&status=1`;
var Off = `appliance=${device}&status=0`;

    if (statusElement.classList.contains('status-off')) {

        xhr.open('GET', "/update?"+On ,true);
        xhr.send();
    } else {
        xhr.open('GET', "/update?"+Off ,true);
        xhr.send();
    }
}

function getPower(device){

    xhr.onreadystatechange = function () {
        if (xhr.readyState === 4 && xhr.status === 200) {
            let response= JSON.parse(xhr.response);
            let graphs = document.getElementsByClassName("cls-7");

            

            if(device==="geyser"){
                
                for(let i=0; i < 7; i++){
                    let height = (response[i+2]/5000)*500;
                    graphs[i].setAttribute("height",`${height}`);
                }
            }
            if(device==="AC"){
                for(let i=7; i < 14; i++){
                    let height = (response[i-5]/5000)*500;
                    graphs[i].setAttribute("height",`${height}`);
                }
            }
            
            
        }
    };


    var get = `?appliance=${device}`;
    xhr.open('GET', '/fetchpower'+get, true);
    
    xhr.send();
}