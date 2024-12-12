import pandas as pd
from sklearn.model_selection import train_test_split

def load_split_and_save_data(csv_file, target_label, test_size=0.2, random_state=42):
    """
    Loads a CSV file, separates features and target, performs a train-test split,
    and saves the split datasets to files.

    Parameters:
    - csv_file (str): Path to the CSV file.
    - target_label (str): The column name of the target label.
    - test_size (float): The proportion of data to include in the test split (default 0.2).
    - random_state (int): Random seed for reproducibility (default 42).

    Saves:
    - diabetes_train.csv: Train dataset.
    - diabetes_test.csv: Test dataset.
    """
    # Load the CSV file into a DataFrame
    data = pd.read_csv(csv_file)
    
    # Check if the target label exists in the data
    if target_label not in data.columns:
        raise ValueError(f"Target label '{target_label}' not found in the CSV file.")
    
    # Separate features (X) and target (y)
    X = data.drop(columns=[target_label])
    y = data[target_label]
    
    # Perform the train-test split
    X_train, X_test, y_train, y_test = train_test_split(X, y, test_size=test_size, random_state=random_state)
    
    # Combine X and y back into DataFrames for saving
    train_data = pd.concat([X_train, y_train], axis=1)
    test_data = pd.concat([X_test, y_test], axis=1)
    
    # Save the datasets to CSV files
    train_data.to_csv("diabetes_train.csv", index=False)
    test_data.to_csv("diabetes_test.csv", index=False)
    print("Train and test datasets saved as 'diabetes_train.csv' and 'diabetes_test.csv'.")
    
    return X_train, X_test, y_train, y_test

# Example usage
if __name__ == "__main__":
    # Replace 'your_file.csv' with the actual file path and 'target_column' with your target label
    csv_file = '../dw-excercise/final-data-transform - final-data-transform.csv'
    target_label = 'readmitted'
    
    try:
        X_train, X_test, y_train, y_test = load_split_and_save_data(csv_file, target_label)
        print("Data successfully loaded, split, and saved!")
    except ValueError as e:
        print(e)
