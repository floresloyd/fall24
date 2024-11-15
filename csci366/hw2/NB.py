import os 
import time
from preprocess import preprocess
from methods import compute_prior_probabilities, compute_conditional_probabilities_with_add1, validate_predictions, get_validation_labels



def nb(train_data_path, test_data_path):
    print("Entering Naive Bayes ... ")
    start_time = time.time()  # record time

    # Extract the subdirectories dynamically
    subdirectories = [d for d in os.listdir(train_data_path) if os.path.isdir(os.path.join(train_data_path, d))]
    
    if len(subdirectories) != 2:
        print("Expected exactly 2 subdirectories within the train_data_path.")
        return
    
    # Extract paths for the two folders
    class1_path = os.path.join(train_data_path, subdirectories[0])
    class2_path = os.path.join(train_data_path, subdirectories[1])

    # PRIOR PROB
    print("\nComputing Prior Probabilities...")
    class1_name, class1_prior, class2_name, class2_prior = compute_prior_probabilities(train_data_path)
    print(f"Prior Probabilities: {class1_name} {class1_prior}, {class2_name} {class2_prior}")

    # Getting BOWs (Bag of Words)
    print("\nGenerating Bag of Words for each class...")
    class1_bow = preprocess(class1_path)
    print(f"{class1_name.capitalize()} BOW: {class1_bow}")

    print()
    class2_bow = preprocess(class2_path)
    print(f"{class2_name.capitalize()} BOW: {class2_bow}")
    print()

    # Getting conditional probabilities
    print("\nComputing Conditional Probabilities with Add-1 Smoothing...")
    class1_probs, class1_bowsize, class2_probs, class2_bowsize, vocabulary_size = compute_conditional_probabilities_with_add1(class1_bow, class2_bow)

    print()
    print(f"BOW1size: {class1_bowsize} BOW2size: {class2_bowsize} VOCAB SIZE: {vocabulary_size}")

    # Creating/Clearing predictions.txt
    with open("predictions.txt", "w") as output_file:
        output_file.write("Making predictions on the files\n")
    
    # Predicting
    for folder in os.listdir(test_data_path):
        folder_path = os.path.join(test_data_path, folder)
        if os.path.isdir(folder_path):
            with open("predictions.txt", "a") as output_file:
                output_file.write(f"--{folder.capitalize()}\n")

            for file_name in os.listdir(folder_path):
                file_path = os.path.join(folder_path, file_name)
                if os.path.isfile(file_path):
                    with open(file_path, 'r', encoding='utf-8') as file:
                        tokens = file.read().split()  # Simple tokenization by whitespace

                    class1_prob = class1_prior
                    class2_prob = class2_prior

                    # Calculate probabilities for class 1
                    for token in tokens:
                        if token in class1_probs:
                            class1_prob *= class1_probs[token]
                        else:
                            class1_prob *= 1 / (class1_bowsize + vocabulary_size)

                    # Calculate probabilities for class 2
                    for token in tokens:
                        if token in class2_probs:
                            class2_prob *= class2_probs[token]
                        else:
                            class2_prob *= 1 / (class2_bowsize + vocabulary_size)

                    # Make prediction (1 for class1, 0 for class2)
                    predicted_label = 1 if class1_prob > class2_prob else 0
                    feature_probs = max(class1_prob, class2_prob)  # Use the max probability for display

                    # Write to predictions file
                    with open("predictions.txt", "a") as output_file:
                        output_file.write(f"{predicted_label}              {feature_probs:.8f}\n")

    # Creating parameters.txt file to save model parameters
    with open("parameters.txt", "w") as param_file:
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





## Driver Code 

# Train and Test File Paths
train_filepath = "./movie-review-small-data/train"
test_filepath = "./movie-review-small-data/test"
# Classes to predict will be used for labels
class1_folder_path = "./movie-review-small-data/test/action"
class2_folder_path = "./movie-review-small-data/test/comedy"
# Output Files
actual_labels = "./labels.txt"
prediction_file = "./predictions.txt"
output_file = "./labels.txt"

# Train and Predict NB
nb(train_filepath, test_filepath)
# Get Labels
get_validation_labels(class1_folder_path, class2_folder_path, output_file)
#Validate
print()
validate_predictions(actual_labels, prediction_file)








