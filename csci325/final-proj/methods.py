import os  # To handle file paths and directories
import xml.etree.ElementTree as ET  # To parse XML files containing annotations
import numpy as np
import cv2


# Function to parse annotations for a given dataset split (e.g., train, val)
def parse_annotations(annotation_dir, image_dir, imageset_file):
    """
    Parses the PASCAL VOC dataset annotations and extracts the image paths 
    and corresponding object class labels.

    Parameters:
    - annotation_dir (str): Directory containing XML annotation files.
    - image_dir (str): Directory containing the images (JPEGImages folder).
    - imageset_file (str): Path to the text file listing image IDs for a dataset split.

    Returns:
    - annotations (list of tuples): A list of tuples where each tuple contains:
        - image_path (str): Path to the image.
        - labels (set of str): Set of object class labels for the image.
    """
    # Read the image IDs from the specified imageset file (e.g., train.txt)
    with open(imageset_file, 'r') as file:
        image_ids = [line.strip() for line in file.readlines()]  # List of image IDs (e.g., 2007_000027)

    annotations = []  # List to store image paths and corresponding labels
    
    # Iterate through each image ID to extract its annotations
    for image_id in image_ids:
        # Construct the path to the XML annotation file for the current image
        xml_file = os.path.join(annotation_dir, f"{image_id}.xml")
        
        # Parse the XML file
        tree = ET.parse(xml_file)  # Load the XML file as a tree structure
        root = tree.getroot()  # Get the root element of the XML file
        
        # Extract object class labels for the current image
        labels = set()  # Use a set to ensure each class is unique
        for obj in root.findall("object"):  # Find all <object> tags in the XML
            class_name = obj.find("name").text  # Extract the class name (e.g., "person", "car")
            labels.add(class_name)  # Add the class name to the set of labels
        
        # Construct the path to the image file in the JPEGImages directory
        image_path = os.path.join(image_dir, f"{image_id}.jpg")
        
        # Append the image path and its corresponding labels to the annotations list
        annotations.append((image_path, labels))
    
    # Return the list of annotations
    return annotations


def parse_animal_annotations(annotation_dir, image_dir, imageset_file, target_classes):
    """
    Parses annotations and filters only the images containing animal classes.

    Parameters:
    - annotation_dir (str): Directory containing XML annotation files.
    - image_dir (str): Directory containing the images (JPEGImages folder).
    - imageset_file (str): Path to the text file listing image IDs for a dataset split.
    - target_classes (list): List of target classes (e.g., animal classes).

    Returns:
    - annotations (list of tuples): A filtered list of (image_path, labels) tuples:
        - image_path (str): Path to the image.
        - labels (set): Set of animal class labels for the image.
    """
    # Read image IDs from the specified imageset file
    with open(imageset_file, 'r') as file:
        image_ids = [line.strip() for line in file.readlines()]

    annotations = []  # Initialize a list to store results
    
    # Iterate through image IDs to extract annotations
    for image_id in image_ids:
        # Locate the corresponding XML file for the current image
        xml_file = os.path.join(annotation_dir, f"{image_id}.xml")
        
        # Parse the XML file
        tree = ET.parse(xml_file)
        root = tree.getroot()
        
        # Extract object class labels
        labels = set()
        for obj in root.findall("object"):
            class_name = obj.find("name").text
            # Add the class name to labels only if it's in the target classes
            if class_name in target_classes:
                labels.add(class_name)
        
        # Skip images with no relevant animal classes
        if not labels:
            continue
        
        # Locate the corresponding image file
        image_path = os.path.join(image_dir, f"{image_id}.jpg")
        
        # Append the image path and its labels to the annotations list
        annotations.append((image_path, labels))
    
    return annotations


def preprocess_images(data, size=(224, 224)):
    """
    Resizes and normalizes images to a fixed size.

    Parameters:
    - data (list of tuples): Each tuple contains (image_path, labels).
    - size (tuple): Desired image size (height, width).

    Returns:
    - images (numpy array): Array of preprocessed images.
    """
    images = []
    for image_path, _ in data:
        # Read the image
        image = cv2.imread(image_path)
        
        if image is None:
            print(f"Warning: Could not read {image_path}")
            continue
        
        # Resize the image
        image = cv2.resize(image, size)
        
        # Normalize pixel values to [0, 1]
        image = image / 255.0
        
        # Append the preprocessed image
        images.append(image)
    
    return np.array(images)



def convert_labels(data, class_list):
    """
    Converts labels (sets of class names) to multi-hot vectors.

    Parameters:
    - data (list of tuples): Each tuple contains (image_path, labels).
    - class_list (list): List of all possible classes.

    Returns:
    - label_vectors (numpy array): Array of multi-hot label vectors.
    """
    # Create a mapping from class names to indices
    class_map = {cls: idx for idx, cls in enumerate(class_list)}
    
    label_vectors = []
    for _, labels in data:
        # Initialize a zero vector for the label
        vector = [0] * len(class_list)
        
        # Set indices corresponding to the labels to 1
        for label in labels:
            if label in class_map:
                vector[class_map[label]] = 1
        
        label_vectors.append(vector)
    
    return np.array(label_vectors)



