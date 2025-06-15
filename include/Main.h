#define MISCOUNT_VERSION "1.0.2"

typedef struct Args {
	char *nameOfMiscount;
	char *nameOfOffender;
	char *descriptionOfMiscount;
} Args;

typedef struct FurtherOptions {
	bool writeDescriptionInEditor;
	bool writeToExistingMiscountFile;
} FurtherOptions;
