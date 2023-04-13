// Copyright © 2023 Hayden Heroux

/* Define `State` enum to keep track of program state. */
typedef enum {
	IDLING, // Perform no action; "do nothing"
	RESETTING, // Reset program to initial state
	UPDATING_RATE, // Update `rate` variable
	UPDATING_SETPOINT, // Update `setpoint` variable
	APPROACHING // Update `measurement` variable
} State;

State state = IDLING; // Start program in the `IDLING` state

int rate = 0; // Absolute rate of change for the `measurement` variable
int setpoint = 0; // Setpoint for the `measurement` variable to approach

int measurement = 0; // Variable to be modified to approach the `setpoint` variable

/* Pin number for the button that, when held, allows the user to update the rate.  */
const int RATE_PIN = 2;
/* Pin number for the button that, when held, allows the user to update the setpoint. */
const int SETPOINT_PIN = 4;
/* Pin number for the potentiometer that set both the rate and the setpoint. */
const int POTENTIOMETER_PIN = A5;
/* Pin number for the LED that indicates whether the program state is `APPROACHING`. */
const int LED_PIN = 13;
/* Tolerance (maximum difference between the setpoint and measurement) for determining if the setpoint needs to be approached.  */
const int TOLERANCE = 5;

int rate_button_held = 0; // Boolean for if the rate button is held
int setpoint_button_held = 0; // Boolean for if the setpoint button is held
int potentiometer_value = 0; // Integer for the value of the potentiometer 

void setup() {
	/* Configure buttons to be inputs. */
	pinMode(RATE_PIN, INPUT);
	pinMode(SETPOINT_PIN, INPUT);

	/* Configure LED to be an output. */
	pinMode(LED_PIN, OUTPUT);
}

void loop() {
	/* Read all input sources. */
	rate_button_held = digitalRead(RATE_PIN);
	setpoint_button_held = digitalRead(SETPOINT_PIN);
	potentiometer_value = analogRead(POTENTIOMETER_PIN);

	/* Update program state depending on input sources. */
	if (rate_button_held && setpoint_button_held) {
		state = RESETTING;
	} else if (rate_button_held) {
		state = UPDATING_RATE;
	} else if (setpoint_button_held) {
		state = UPDATING_SETPOINT;
	} else if (abs(setpoint - measurement) > TOLERANCE) {
		state = APPROACHING;
	} else {
		state = IDLING;
	}

	/* Perform program functions depending on program state. */
	switch (state) {
		case RESETTING:
		/* Reset program to initial state. */
			measurement = 0;
			setpoint = 0;
			rate = 0;
			digitalWrite(LED_PIN, LOW);
			break;
		case UPDATING_RATE:
		/* Update `rate` variable. */
			rate = potentiometer_value;
			rate /= 128;
			digitalWrite(LED_PIN, LOW);
			break;
		case UPDATING_SETPOINT:
		/* Update `setpoint` variable. */
			setpoint = potentiometer_value;
			digitalWrite(LED_PIN, LOW);
			break;
		case APPROACHING:
		/* Update `measurement` variable. */
			// Correct the sign of the `rate` variable to prevent overrun
			if (rate > 0 && measurement > setpoint) rate *= -1;
			if (rate < 0 && measurement < setpoint) rate *= -1;
			measurement += rate;
			digitalWrite(LED_PIN, HIGH);
			break;
		case IDLING:
		default:
		/* Perform no action; "do nothing." */
			digitalWrite(LED_PIN, LOW);
			break;
	}
}
