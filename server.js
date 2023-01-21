const express = require('express');
const bodyParser = require('body-parser');
var fs = require("file-system");
const { PredictionServiceClient } = require("@google-cloud/automl").v1;

const projectId = "green-source-345711";
const location = "us-central1";
const modelId = "ICN4221510023659913216";
const client = new PredictionServiceClient();

const app = express();
const port = 8080;

app.use(bodyParser.json());
app.use(bodyParser.urlencoded({ extended: true }));

app.listen(8080, ()=> console.log(`>>Server listening at: ${port}`));

app.get('/', (req, res)=>{
  res.status(200).json({});
})

const filePath = "./resources/espcam.jpeg";
var imageFile = fs.createWriteStream(filePath, { enconding: "utf8" });

app.post('/imageUpdate', (req, res)=>{  
  req.on("data", (data) => {
    const date = new Date(Date.now());
    console.log(`Request received: ${date}`);
    imageFile.write(data);
  });
  
  req.on("end", async ()=> {
    imageFile.end();
    const labels = await predict();
    console.log(labels);
    res.status(200).json(labels);
  })
})

async function predict() {
  const content = fs.readFileSync(filePath);

  const request = {
    name: client.modelPath(projectId, location, modelId),
    payload: {
      image: {
        imageBytes: content,
      },
    },
  };

  const [response] = await client.predict(request);
  const data = [];
  for (const annotationPayload of response.payload) {
    data.push({description: annotationPayload.displayName, score: annotationPayload.classification.score});
  }

  return data;
}
