/**
 * TODO(developer): Uncomment these variables before running the sample.
 */
 const projectId = "green-source-345711";
 const location = "us-central1";
 const modelId = "ICN4221510023659913216";
 const filePath = './resources/capacete_oculos_3.jpeg';
//  const filePath = './resources/sem_nada_0.jpeg';

// Imports the Google Cloud AutoML library
const {PredictionServiceClient} = require('@google-cloud/automl').v1;
const fs = require('fs');

// Instantiates a client
const client = new PredictionServiceClient();

// Read the file content for translation.
const content = fs.readFileSync(filePath);

async function predict() {
  // Construct request
  // params is additional domain-specific parameters.
  // score_threshold is used to filter the result
  const request = {
    name: client.modelPath(projectId, location, modelId),
    payload: {
      image: {
        imageBytes: content,
      },
    },
  };

  const [response] = await client.predict(request);

  for (const annotationPayload of response.payload) {
    console.log(`Predicted class name: ${annotationPayload.displayName}`);
    console.log(
      `Predicted class score: ${annotationPayload.classification.score}`
    );
  }
}

predict();