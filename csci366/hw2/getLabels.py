import os

def get_validation_labels(class1_folder_path, class2_folder_path, output_file):
    with open(output_file, "w") as file:
        file.write("Actual Labels\n")
        
        # Write for Class 1
        file.write(f"--1\n")
        for file_name in os.listdir(class1_folder_path):
            if os.path.isfile(os.path.join(class1_folder_path, file_name)):
                file.write(f"1 {file_name}\n")
        
        # Write for Class 2
        file.write(f"--0\n")
        for file_name in os.listdir(class2_folder_path):
            if os.path.isfile(os.path.join(class2_folder_path, file_name)):
                file.write(f"0 {file_name}\n")



def validate_predictions(actual_labels_file, predictions_file):
    # Read the actual labels into a list
    with open(actual_labels_file, "r") as actual_file:
        actual_lines = [line.strip() for line in actual_file if not line.startswith("Actual Labels") and not line.startswith("--")]

    # Read the predictions into a list
    with open(predictions_file, "r") as pred_file:
        pred_lines = [line.strip() for line in pred_file if not line.startswith("Making predictions") and not line.startswith("--")]

    correct_predictions = 0
    total_predictions = 0

    # Compare actual labels with predictions
    for actual, prediction in zip(actual_lines, pred_lines):
        actual_label, actual_filename = actual.split()
        predicted_label, _ = prediction.split(maxsplit=1)  # Ignore the second column (probability)

        total_predictions += 1
        if actual_label == predicted_label:
            correct_predictions += 1

    # Calculate accuracy
    accuracy = (correct_predictions / total_predictions) * 100 if total_predictions > 0 else 0
    print(f"Validation Completed: {correct_predictions}/{total_predictions} correct predictions.")
    print(f"Accuracy: {accuracy:.2f}%")







