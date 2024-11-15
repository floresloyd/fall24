import os 
import time

def compute_prior_probabilities(data_path):
    print(f"Computing Prior Probabilities for {data_path} ...")
    start_time = time.time()  # record time

    total_docs = 0
    class_doc_counts = {}

    # Traverse through all directories (classes)
    for root, dirs, files in os.walk(data_path):
        if root != data_path:
            class_name = os.path.basename(root)
            num_docs = len(files)
            total_docs += num_docs
            class_doc_counts[class_name] = num_docs

    if len(class_doc_counts) != 2:
        raise ValueError("Expected exactly 2 classes within the data_path.")

    class_names = list(class_doc_counts.keys())
    num_docs_class1 = class_doc_counts[class_names[0]]
    num_docs_class2 = class_doc_counts[class_names[1]]

    prior_prob1 = num_docs_class1 / total_docs if total_docs > 0 else 0
    prior_prob2 = num_docs_class2 / total_docs if total_docs > 0 else 0

    elapsed_time = time.time() - start_time
    print(f"compute_prior_probabilities() executed in {elapsed_time:.4f} seconds")

    return class_names[0], prior_prob1, class_names[1], prior_prob2

def compute_conditional_probabilities_with_add1(bow1, bow2):
    print(f"Computing conditional probabilities and smoothing ...")
    start_time = time.time() 

    bow1_size = sum(bow1.values())
    bow2_size = sum(bow2.values())
    vocabulary_size = bow1_size + bow2_size

    conditional_probabilities1 = {token: (count + 1) / (bow1_size + vocabulary_size) for token, count in bow1.items()}
    conditional_probabilities2 = {token: (count + 1) / (bow2_size + vocabulary_size) for token, count in bow2.items()}

    elapsed_time = time.time() - start_time
    print(f"compute_conditional_probabilities_with_add1() executed in {elapsed_time:.4f} seconds")

    return conditional_probabilities1, bow1_size, conditional_probabilities2, bow2_size, vocabulary_size

def get_validation_labels(class1_folder_path, class2_folder_path, output_file):
    start_time = time.time()
    with open(output_file, "w") as file:
        file.write("Extracting Ground Truth\n")

        # Ground truth for Class 1 (Label 1)
        file.write(f"--{os.path.basename(class1_folder_path).capitalize()}\n")
        for file_name in sorted(os.listdir(class1_folder_path)):
            if os.path.isfile(os.path.join(class1_folder_path, file_name)):
                file.write(f"1 {file_name}\n")  # No probability, just label and file name

        # Ground truth for Class 2 (Label 0)
        file.write(f"--{os.path.basename(class2_folder_path).capitalize()}\n")
        for file_name in sorted(os.listdir(class2_folder_path)):
            if os.path.isfile(os.path.join(class2_folder_path, file_name)):
                file.write(f"0 {file_name}\n")  # No probability, just label and file name

    elapsed_time = time.time() - start_time
    print(f"get_validation_labels() executed in {elapsed_time:.4f} seconds")



def validate_predictions(actual_labels_file, predictions_file):
    start_time = time.time()
    with open(actual_labels_file, "r") as actual_file:
        actual_lines = sorted([line.strip() for line in actual_file if not line.startswith("Actual Labels") and not line.startswith("--")])

    with open(predictions_file, "r") as pred_file:
        pred_lines = sorted([line.strip() for line in pred_file if not line.startswith("Making predictions") and not line.startswith("--")])

    correct_predictions = sum(1 for actual, prediction in zip(actual_lines, pred_lines) if actual.split()[0] == prediction.split()[0])
    total_predictions = len(pred_lines)

    accuracy = (correct_predictions / total_predictions) * 100 if total_predictions > 0 else 0
    print(f"Validation Completed: {correct_predictions}/{total_predictions} correct predictions.")
    print(f"Accuracy: {accuracy:.2f}%")
    elapsed_time = time.time() - start_time
    print(f"validate_predictions() in {elapsed_time:.4f} seconds")



def ensure_directory_exists(file_path):
    directory = os.path.dirname(file_path)
    if not os.path.exists(directory):
        os.makedirs(directory)