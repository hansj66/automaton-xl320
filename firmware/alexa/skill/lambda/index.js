/* *
 * This sample demonstrates handling intents from an Alexa skill using the Alexa Skills Kit SDK (v2).
 * Please visit https://alexa.design/cookbook for additional examples on implementing slots, dialog management,
 * session persistence, api calls, and more.
 * */
const Alexa = require('ask-sdk-core');

const LaunchRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'LaunchRequest';
    },
    async handle(handlerInput) {
        const speakOutput = 'Launching project overlord. Are you sure you want to do this?';
        
        
////////////////////////////////        
        let response;
        try {
            // Get connected gadget endpointId.
            console.log("Checking endpoint");
            response = await getConnectedEndpointsResponse(handlerInput);
            console.log("v1/endpoints response: " + JSON.stringify(response));

            let num_end_points_found = (response.endpoints || []).length;
            if ((response.endpoints || []).length === 0) {
                console.log('No connected gadget endpoints available');
                response = handlerInput.responseBuilder
                    .speak("Darn, no gadgets found. Please try again after connecting your gadget.")
                    .getResponse();
                return response;
            }

            let endpointId = response.endpoints[0].endpointId;

            // Store endpointId for using it to send custom directives later.
            console.log("Received endpoints. Storing Endpoint Id: " + endpointId);
            const attributesManager = handlerInput.attributesManager;
            let sessionAttributes = attributesManager.getSessionAttributes();
            sessionAttributes.endpointId = endpointId;
            attributesManager.setSessionAttributes(sessionAttributes);

            return handlerInput.responseBuilder
                .speak(speakOutput)
                //.speak('Lucky you. Seems like you are a lambda coding God after all. I found ' + num_end_points_found + 'endpoint')
                .reprompt('add a reprompt if you want to keep the session open for the user to respond')
                .getResponse();
        }
        catch (err) {
                response = handlerInput.responseBuilder
                    .speak("I wasn't able to get connected endpoints. You are an old fart that cannot code Node. Your code sucks.")
                    .withShouldEndSession(true)
                    .getResponse();
                return response;
        }
    }        
        
        
////////////////////        
        
        
        
        
        
        

//        return handlerInput.responseBuilder
//            .speak(speakOutput)
//            .reprompt(speakOutput)
//            .getResponse();
//    }
};


function getConnectedEndpointsResponse(handlerInput) {
    return handlerInput.serviceClientFactory.getEndpointEnumerationServiceClient().getEndpoints();
}


const StartRobotRequestHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'StartRobotIntent';
    },
    handle(handlerInput) {
        let response;
        const speakOutput = 'Greetings! I was able to locate your connected gadget endpoint. I guess that we will have much fun together in the near future!';
        
        console.log(JSON.stringify(handlerInput.requestEnvelope));

        try {
            // Get connected gadget endpointId.
            console.log("Checking endpoint");
            response = getConnectedEndpointsResponse(handlerInput);
            // response =  async() => {return handlerInput.serviceClientFactory.getEndpointEnumerationServiceClient().getEndpoints()};
            console.log("v1/endpoints response: " + JSON.stringify(response));

            if ((response.endpoints || []).length === 0) {
                console.log('No connected gadget endpoints available');
                response = handlerInput.responseBuilder
                    .speak("No gadgets found. Please try again after connecting your gadget.")
                    .getResponse();
                return response;
            }

            let endpointId = response.endpoints[0].endpointId;

            // Store endpointId for using it to send custom directives later.
            console.log("Received endpoints. Storing Endpoint Id: " + endpointId);
            const attributesManager = handlerInput.attributesManager;
            let sessionAttributes = attributesManager.getSessionAttributes();
            sessionAttributes.endpointId = endpointId;
            attributesManager.setSessionAttributes(sessionAttributes);

            return handlerInput.responseBuilder
                .speak(speakOutput)
                //.reprompt('add a reprompt if you want to keep the session open for the user to respond')
                .getResponse();
        }
        catch (err) {
                response = handlerInput.responseBuilder
                    .speak("I wasn't able to get connected endpoints. You are an old fart that cannot code Node. Your code sucks.")
                    .withShouldEndSession(true)
                    .getResponse();
                return response;
        }
    }
};


const CustomInterfaceEventHandler = {
    canHandle(handlerInput) {
        let { request } = handlerInput.requestEnvelope;
        console.log("CustomEventHandler: checking if it can handle " + request.type);
        return request.type === 'CustomInterfaceController.EventsReceived';
    },
    handle(handlerInput) {
        console.log("== Received Custom Event ==");

        let { request } = handlerInput.requestEnvelope;

        const attributesManager = handlerInput.attributesManager;
        let sessionAttributes = attributesManager.getSessionAttributes();

        // Validate eventHandler token
        if (sessionAttributes.token !== request.token) {
            console.log("EventHandler token doesn't match. Ignoring this event.");
            return handlerInput.responseBuilder
                .speak("EventHandler token doesn't match. Ignoring this event.")
                .getResponse();
        }

        let customEvent = request.events[0];
        let payload = customEvent.payload;
        let namespace = customEvent.header.namespace;
        let name = customEvent.header.name;

        let response = handlerInput.responseBuilder;

        if (namespace === 'Custom.ColorCyclerGadget' && name === 'ReportColor') {
            // On receipt of 'Custom.ColorCyclerGadget.ReportColor' event, speak the reported color
            // and end skill session.
            return response.speak(payload.color + ' is the selected color. Thank you for playing. Good bye!')
                .withShouldEndSession(true)
                .getResponse();
        }
            return response;
    }
};
    
const CustomInterfaceExpirationHandler = {
    canHandle(handlerInput) {
        let { request } = handlerInput.requestEnvelope;
        console.log("CustomEventHandler: checking if it can handle " + request.type);
        return request.type === 'CustomInterfaceController.Expired';
    },
    handle(handlerInput) {
        console.log("== Custom Event Expiration Input ==");

        let { request } = handlerInput.requestEnvelope;

        const attributesManager = handlerInput.attributesManager;
        let sessionAttributes = attributesManager.getSessionAttributes();

        // When the EventHandler expires, send StopLED directive to stop LED animation
        // and end skill session.
        return handlerInput.responseBuilder
//            .addDirective(buildStopLEDDirective(sessionAttributes.endpointId))
            .withShouldEndSession(true)
            .speak(request.expirationPayload.data)
            .getResponse();
    }
}


const HelloWorldIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'HelloWorldIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Hello world.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            //.reprompt('add a reprompt if you want to keep the session open for the user to respond')
            .getResponse();
    }
};

const HelpIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.HelpIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'How can I best please you, master?';

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
        const speakOutput = 'What are you doing, Dave!';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .getResponse();
    }
};
/* *
 * FallbackIntent triggers when a customer says something that doesn’t map to any intents in your skill
 * It must also be defined in the language model (if the locale supports it)
 * This handler can be safely added but will be ingnored in locales that do not support it yet 
 * */
const FallbackIntentHandler = {
    canHandle(handlerInput) {
        return Alexa.getRequestType(handlerInput.requestEnvelope) === 'IntentRequest'
            && Alexa.getIntentName(handlerInput.requestEnvelope) === 'AMAZON.FallbackIntent';
    },
    handle(handlerInput) {
        const speakOutput = 'Sorry, I don\'t know about that. Please try again.';

        return handlerInput.responseBuilder
            .speak(speakOutput)
            .reprompt(speakOutput)
            .getResponse();
    }
};
/* *
 * SessionEndedRequest notifies that a session was ended. This handler will be triggered when a currently open 
 * session is closed for one of the following reasons: 1) The user says "exit" or "quit". 2) The user does not 
 * respond or says something that does not match an intent defined in your voice model. 3) An error occurs 
 * */
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
/* *
 * The intent reflector is used for interaction model testing and debugging.
 * It will simply repeat the intent the user said. You can create custom handlers for your intents 
 * by defining them above, then also adding them to the request handler chain below 
 * */
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
/**
 * Generic error handling to capture any syntax or routing errors. If you receive an error
 * stating the request handler chain is not found, you have not implemented a handler for
 * the intent being invoked or included it in the skill builder below 
 * */
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

/**
 * This handler acts as the entry point for your skill, routing all request and response
 * payloads to the handlers above. Make sure any new handlers or interceptors you've
 * defined are included below. The order matters - they're processed top to bottom 
 * */
exports.handler = Alexa.SkillBuilders.custom()
    .addRequestHandlers(
        LaunchRequestHandler,
        StartRobotRequestHandler,
        CustomInterfaceEventHandler,
        CustomInterfaceExpirationHandler,
        HelloWorldIntentHandler,
        HelpIntentHandler,
        CancelAndStopIntentHandler,
        FallbackIntentHandler,
        SessionEndedRequestHandler,
        IntentReflectorHandler)
    .addErrorHandlers(
        ErrorHandler)
    .withApiClient(new Alexa.DefaultApiClient())
    .withCustomUserAgent('sample/hello-world/v1.2')
    .lambda();