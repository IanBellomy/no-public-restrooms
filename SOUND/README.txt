Each 'BLINK#' folder corresponds to an SD card.

! Sound files MUST named 001.mp3 etc. in a folder named '01'. So far, anything else seems to impact the actual order the Aideepen thinks they're in which impacts what sounds will play with what event.

The sound file numbers correspond to events:

	001	: sit continue - plays when the user sits until the sit satisfied event
	002	: sit satisfied - plays when the user has sat long enough (about 6 seconds)
	003 : sit complete - plays when the user stands after having sat long enough

	004 : bonus idle - plays every so often if no one is interacting with the toilet [optional]
	005 : alt sit continue - eventually will be used as an variation for 002 [optional, though 004 is required for this to work]
	006 : sit cancel - plays when a sitting person stands before having sat long enough for 'sit satisfied' [optional, though 004 and 005 are required for this to work]

! The 01 folder with all sounds MUST be placed on the SD card in one go -- attempting to add, remove, or modify sound files on the SD card itself might change the order they're read by the Aideepen.

If working from OSX, make sure there are no private/secrete system files/folders starting with "." on the SD card. The SD card MUST only contain the '01' folder with sounds.