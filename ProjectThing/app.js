// JSON Data Format
/*

[
    {
        "runid": 1,
        "lat": [],
        "lon": []
    }
]

*/

//

const path = require('path')
const express = require('express')
fs = require('fs').promises;
util = require('util');
writeFile = util.promisify(fs.writeFile);

const app = express()
const router = express.Router();
app.use(express.static(__dirname));
const port = process.env.PORT || 3000

app.set("view engine", "ejs");
app.use(express.static(__dirname));

writeToFile = async (file, data) => { await writeFile(file, data); }

// ----------------------------------------------------------------

app.get('/append/:runid/:lat/:lon', (req, res) => {
    const main = async () => {

        var runid = req.params.runid
        var lat = req.params.lat
        var lon = req.params.lon

        var data1 = await fs.readFile('/root/server/run.json', 'utf8');
        data1 = JSON.parse(data1)

        var newlat = data1[data1.length-1].lat
        var newlon = data1[data1.length-1].lon
        
        newlat.push(parseFloat(lat))
        newlon.push(parseFloat(lon))

        var data2 = {
            runid: parseInt(runid),
            lat: newlat,
            lon: newlon
        }

        data1.splice(-1)
        data1.push(data2)

        await fs.writeFile('/root/server/run.json', JSON.stringify(data1));
        var data = await fs.readFile('/root/server/run.json', 'utf8');
        myObject = JSON.parse(data)
        console.log(lat + " " + lon)

        res.send('Done')

    }
    main()
});

app.get('/newrun', (req, res) => {
    const main = async () => {

        var data1 = await fs.readFile('/root/server/run.json', 'utf8');
        data1 = JSON.parse(data1)
        lastrun = data1[data1.length-1]
        
        var data2 = {
            "runid": (parseInt(lastrun.runid)+1),
            "lat": [],
            "lon": []
        }
        data1.push(data2)

        await fs.writeFile('/root/server/run.json', JSON.stringify(data1));

        console.log('New Run Created')
        res.send(String(parseInt(lastrun.runid)+1))

    }
    main()
});

app.get('/resetfile', (req, res) => {
    const main = async () => {

        var data = [
            {
                "runid": 1,
                lat: [],
                lon: []
            }
        ]

        await fs.writeFile('/root/server/run.json', JSON.stringify(data));
        console.log('Action - File reset')
        res.send('File Reset')

    }
    main()
});

app.get('/getNumRuns', (req, res) => {
    const main = async () => {

        var runData = await fs.readFile('/root/server/run.json', 'utf8');
        runData = JSON.parse(runData)

        res.header("Access-Control-Allow-Origin", "*")
        res.json(
            {
                numRuns: parseInt(runData.length)
            }
        )

    }
    main()
});

app.get('/analyze-run/:runNum', (req, res) => {
    const main = async () => {

        var runNum = req.params.runNum

        var runData = await fs.readFile('/root/server/run.json', 'utf8');
        runData = JSON.parse(runData)

        for(let i = 0; i < runData.length; i++) {
            if(runData[i].runid == runNum){
                var retRunID = runData[i].runid
                var retLat = runData[i].lat
                var retLon = runData[i].lon
            }
        }

        res.header("Access-Control-Allow-Origin", "*")
        res.json(
            {
                runid: retRunID,
                lat: retLat,
                lon: retLon
            }
        )

    }
    main()
});

app.listen(port, () => {
    console.log('Server is up on port ' + port)
})