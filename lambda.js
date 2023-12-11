const Alexa = require('ask-sdk-core');
const AWS = require('aws-sdk');
const IotData = new AWS.IotData({endpoint: 'a30kzystkjltf5-ats.iot.us-east-1.amazonaws.com'});


const AbrirParams = {
  topic: '$aws/things/cortina/shadow/update',
  payload: '{"state": {"desired": {"action": "abrir"}}}',
  qos: 0
};

const CerrarParams = {
  topic: '$aws/things/cortina/shadow/update',
  payload: '{"state": {"desired": {"action": "cerrar"}}}',
  qos: 0
};

function getShadowPromise(params) {
    return new Promise((resolve, reject) => {
        IotData.getThingShadow(params, (err, data) => {
            if (err) {
                console.log(err, err.stack);
                reject('Failed to get thing shadow ${err.errorMessage}');
            } else {
                resolve(JSON.parse(data.payload));
            }
        });
    });
}

const ShadowParams = {
    thingName: 'cortina',
};

const QueryEstadoIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'QueryIntent';
    },
    async handle(handlerInput) {
        var action = 'unknown';
        await getShadowPromise(ShadowParams)
          .then((result) => action = result.state.reported.estado);
        console.log(action);

        var speakOutput = 'Error';
        if (action == 'abrir') {
            speakOutput = 'Abriendo';
        } else if (action == 'cerrar') {
            speakOutput = 'Cerrando';
        } else if (action == 'detener') {
            speakOutput = 'Deteniendo';
        }
        else {
            speakOutput = 'No se pudo consultar el estado de la cortina Inteligente';
        }

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
  }
};

const LaunchRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'LaunchRequest';
    },
    handle(handlerInput) {
        const speakOutput = 'Bienvenido al control de tus Cortinas';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const AbrirIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AbrirIntent';
    },
    handle(handlerInput) {
        var speakOutput = 'Error';
        IotData.publish(AbrirParams, function(err, data) {
            if(err) console.log(err);
        });
        
        speakOutput = 'Abriendo';
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
  }
};

const CerrarIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'CerrarIntent';
    },
    handle(handlerInput) {
        var speakOutput = 'Error';
        IotData.publish(CerrarParams, function(err, data){
            if (err) console.log(err);
        });
        speakOutput = 'Cerrando';
        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
  }
};

const HelloWorldIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'HelloWorldIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Hello World!';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            
            .getResponse();
    }
};

const HelpIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.HelpIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Tienes la opcion de abrir o cerrar las cortinas';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const CancelAndStopIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && (Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.CancelIntent'
                || Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.StopIntent');
    },
    handle(handlerInput) {
        const speakOutput = 'Cerrando Control de Cortinas!';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .getResponse();
    }
};

const FallbackIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.FallbackIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Lo siento, no sé nada de eso. Inténtalo de nuevo.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};

const SessionEndedRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'SessionEndedRequest';
    },
    handle(handlerInput) {
        console.log(`~~~~ Session ended: ${JSON.stringify(handlerInput.requestEnvelope)}`);
        // Any cleanup logic goes here.
        return handlerInput.responseBuilder.getResponse(); // notice we send an empty response
    }
};

const IntentReflectorHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest';
    },
    handle(handlerInput) {
        const intentName = Alexa.getIntentName(handlerInput.requestEnvelope);
        const speakOutput = `You just triggered ${intentName}`;

        return handlerInput.responseBuilder
            .speak(speakOutput)
            //.reprompt('add a reprompt if you want to keep the session open for the user to respond')
            .getResponse();
    }
};

const ErrorHandler = {
    canHandle() {
        return true;
    },
    handle(handlerInput, error) {
        const speakOutput = 'Sorry, I had trouble doing what you asked. Please try again.';
        console.log(`~~~~ Error handled: ${JSON.stringify(error)}`);

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};
//LLAMADA A FUNCIONES:
exports.handler = Alexa.SkillBuilders.custom()
    .addRequestHandlers(
        LaunchRequestHandler,
        AbrirIntentHandler,
        CerrarIntentHandler,
        QueryEstadoIntentHandler,
        HelloWorldIntentHandler,
        HelpIntentHandler,
        CancelAndStopIntentHandler,
        FallbackIntentHandler,
        SessionEndedRequestHandler,
        IntentReflectorHandler)
    .addErrorHandlers(
        ErrorHandler)
    .withCustomUserAgent('sample/hello-world/v1.2')
    .lambda();
