import os 
import time
from preprocess import preprocess
from methods import compute_prior_probabilities, compute_conditional_probabilities_with_add1, validate_predictions, get_validation_labels

def nb(train_data_path, test_data_path, parameter_filepath, prediction_filepath, label_filepath):
    # Step 1: Log method call 
    print("Entering Naive Bayes ... ")

    # Step 2: Record execution Time
    start_time = time.time()  # record time

    # Step 3: Extract and Sort the subdirectories dynamically for consistent ordering
    subdirectories = sorted([d for d in os.listdir(train_data_path) if os.path.isdir(os.path.join(train_data_path, d))])
    
    if len(subdirectories) != 2:
        print("Expected exactly 2 subdirectories within the train_data_path.")
        return
    
    # Step 4: Compute Prior Probabilities
    class1_path = os.path.join(train_data_path, subdirectories[0])
    class2_path = os.path.join(train_data_path, subdirectories[1])

    print("\nComputing Prior Probabilities...")
    class1_name, class1_prior, class2_name, class2_prior = compute_prior_probabilities(train_data_path)
    print(f"Prior Probabilities: {class1_name} {class1_prior}, {class2_name} {class2_prior}")

    # Step 5: Generate BOW for each Feature
    print("\nGenerating Bag of Words for each class...")
    class1_bow = preprocess(class1_path)
    print(f"{class1_name.capitalize()} BOW: {class1_bow}")

    print()
    class2_bow = preprocess(class2_path)
    print(f"{class2_name.capitalize()} BOW: {class2_bow}")
    print()

    # Step 6: Generate mapping for test classes
    print(f"Class Mapping:\n1 -> {class1_name}\n0 -> {class2_name}")

    # Step 7: Compute conditional probabilities with add 1 smoothing and extract vocabulary size
    print("\nComputing Conditional Probabilities with Add-1 Smoothing...")
    class1_probs, class1_bowsize, class2_probs, class2_bowsize, vocabulary_size = compute_conditional_probabilities_with_add1(class1_bow, class2_bow)

    print()
    print(f"Size of Bag of Word Features for {class1_name.capitalize()}: {class1_bowsize}")
    print(f"Size of Bag of Word Features for {class2_name.capitalize()}: {class2_bowsize}")
    print(f"Vocabulary Size: {vocabulary_size}")

    # Step 8: Generate Predictions
    with open(prediction_filepath, "w") as output_file:
        output_file.write("Making predictions on the files\n")
    
    # Predicting with sorted inputs
    for folder in sorted(os.listdir(test_data_path)):
        folder_path = os.path.join(test_data_path, folder)
        if os.path.isdir(folder_path):
            with open(prediction_filepath, "a") as output_file:
                output_file.write(f"--{folder.capitalize()}\n")

            for file_name in sorted(os.listdir(folder_path)):
                file_path = os.path.join(folder_path, file_name)
                if os.path.isfile(file_path):
                    with open(file_path, 'r', encoding='utf-8') as file:
                        tokens = file.read().split()  # Simple tokenization by whitespace

                    class1_prob = class1_prior
                    class2_prob = class2_prior

                    # Calculate probabilities for class 1
                    for token in tokens:
                        class1_prob *= class1_probs.get(token, 1 / (class1_bowsize + vocabulary_size))

                    # Calculate probabilities for class 2
                    for token in tokens:
                        class2_prob *= class2_probs.get(token, 1 / (class2_bowsize + vocabulary_size))

                    # Make prediction (1 for class1, 0 for class2)
                    predicted_label = 1 if class1_prob > class2_prob else 0
                    feature_probs = max(class1_prob, class2_prob)  # Use the max probability for display

                    # Write to predictions file with the filename as the third column
                    with open(prediction_filepath, "a") as output_file:
                        output_file.write(f"{predicted_label}              {feature_probs:.8f}              {file_name}\n")

    # Step 9: Generate and store model parameters 
    with open(parameter_filepath, "w", encoding='utf-8') as param_file:
        param_file.write(f"{class1_name.capitalize()}\n")
        for token, prob in class1_probs.items():
            param_file.write(f"{token}: {prob:.8f}\n")
        param_file.write("\n")

        param_file.write(f"{class2_name.capitalize()}\n")
        for token, prob in class2_probs.items():
            param_file.write(f"{token}: {prob:.8f}\n")
        param_file.write("\n")

    end_time = time.time()
    elapsed_time = end_time - start_time
    print(f"NB() executed in {elapsed_time:.4f} seconds")
