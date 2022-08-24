var geocoder;
var map;

async function fetchAsync (url) {
    let response = await fetch(url);
    let data = await response.json();
    return data;
};

async function init() {
    var getURL = 'http://176.58.116.164:3000/getNumRuns';
    var res = await fetchAsync(getURL);
    var select = document.getElementById('select-run');
    for(let i = 0; i < res.numRuns; i++){
        var option = document.createElement('option');
        option.text = 'Run ' + String(i + 1);
        option.value = i + 1;
        select.add(option);
    }
};

function distance(lat1, lat2, lon1, lon2) {
    // The math module contains a function
    // named toRadians which converts from
    // degrees to radians.
    lon1 =  lon1 * Math.PI / 180;
    lon2 = lon2 * Math.PI / 180;
    lat1 = lat1 * Math.PI / 180;
    lat2 = lat2 * Math.PI / 180;

    // Haversine formula
    let dlon = lon2 - lon1;
    let dlat = lat2 - lat1;
    let a = Math.pow(Math.sin(dlat / 2), 2)
            + Math.cos(lat1) * Math.cos(lat2)
            * Math.pow(Math.sin(dlon / 2),2);
        
    let c = 2 * Math.asin(Math.sqrt(a));

    // Radius of earth in kilometers. Use 3956
    // for miles
    let r = 6371;

    // calculate the result
    return(c * r);
}

document.getElementById('analyze-data').onclick = async function() {
    var runNum = document.getElementById('select-run').value;
    var getURL = 'http://176.58.116.164:3000/analyze-run/' + runNum
    console.log(getURL)
    var res = await fetchAsync(getURL);

    const map = new google.maps.Map(document.getElementById("map"), {
        zoom: 17,
        center: new google.maps.LatLng(res.lat[0], res.lon[0]),
        mapTypeId: "terrain",
    });
    
    const runCoords = [];

    for(let i = 0; i < res.lat.length; i++){
        runCoords.push(
            {
                lat: res.lat[i],
                lng: res.lon[i]
            }
        )
    }

    const runPath = new google.maps.Polyline({
        path: runCoords,
        geodesic: true,
        strokeColor: "#e34444",
        strokeOpacity: 1.0,
        strokeWeight: 4,
    });
    
    runPath.setMap(map);
};

window.addEventListener('load', function() {
    init();
})