How to run code:
1. Make sure to cd into the 'code' directory
2. Go to main.py 
3. input filepaths in ACTUAL DATA (Test Data will always by default be able to run)
4. Run 
5. Output will be printed on the terminal, label.txt, paramaters.txt, predictions.txt


main():
1. call naive bayes to generate predictions
2. get labels 
3. validate predictions 

def nb(train_data_path, test_data_path):
1. Log "Entering Naive Bayes"
2. Start recording execution time
3. Sort Test Subdirectories for consistent labeling and make code dynamic
4. Compute priorprobabilities for class1, class2 
5. Generate BOW Features for each class 
6. Generate Mapping for test classes
7. Compute Conditional Probabilities with add 1 smoothing and Extract vocabulary size 
8. Generate predictions on test set 
9. Save model parameters 
