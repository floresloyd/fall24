import torch
import torch.nn as nn

class AnimalClassifier(nn.Module):
    def __init__(self, num_classes):
        super(AnimalClassifier, self).__init__()
        '''
        convolutional layers - 2 is a good balance of computational efficiency without losing information

        self.conv1 = nn.Conv2d(Input Channels, Output Channels, Kernel Size, Padding)

        • Input Channels are set to 3, representing the Red Green Blue channels of the input image
            because the dataset consists of colored images.

        • Output Channels differ in the two layers of the CNN. The first layer is set to 32 feature
            maps. This number balances the model’s capacity to learn diverse features while maintaining
            computational efficiency. The second output channel is set to 64 allowing the model to
            capture more complex and abstract features.

        • Kernel Size size of 3x3 was selected as this is the standard choice in convolutional neural
            networks, as it effectively captures local patterns such as edges and textures while keeping
            computational costs low.

        • Padding of 1 ensures that the spatial dimensions of the output match the input, facilitating
            consistent feature extractions across multiple layers. This approach avoids the reduction of
            image dimensions, which could lead to the loss of important spatial information because
            low level computer vision operations operate on neighboring pixels a padding or frame of 1
            to prevent unwanted results.
        '''

        # Convolutional layers
        self.conv1 = nn.Conv2d(3, 32, kernel_size=3, padding=1)
        self.conv2 = nn.Conv2d(32, 64, kernel_size=3, padding=1)


        '''
        pooling layers 
        
        self.pool = nn.MaxPool2d(kernel size, stride)
        
        •Kernel Size 2x2 kernel reduces the spatial dimensions by half, effectively summarizing
            features while reducing computational cost.

        • Stride of 2 ensures non-overlapping pooling, further simplifying the feature maps and
            maintaining efficiency.

        '''
        # Pooling Layers
        self.pool = nn.MaxPool2d(2, 2)
        

        '''
        fully connected layers 
            - Input Size (64 * 56 * 56): This corresponds to the flattened output from the final
                convolutional and pooling layers, where 64 is the number of feature maps, and 56 x 56
                represents the spatial dimensions.
            
            - Output Size (128): A smaller dimensional space is chosen to reduce the feature
                representation while maintaining the models capacity to learn meaningful patterns.

        second layer connects first layer and maps it to number_classes
            

        '''

        # Fully connected layers
        self.fc1 = nn.Linear(64 * 56 * 56, 128)  # Adjust based on input size
        self.fc2 = nn.Linear(128, num_classes)
        
        # Activation functions
        self.relu = nn.ReLU()
        self.sigmoid = nn.Sigmoid()  # For multi-label classification

    def forward(self, x):
        # Convolutional layers
        x = self.pool(self.relu(self.conv1(x)))
        x = self.pool(self.relu(self.conv2(x)))
        
        # Flatten the output
        x = x.view(-1, 64 * 56 * 56)  # Flatten the feature map
        
        # Fully connected layers
        x = self.relu(self.fc1(x))
        x = self.sigmoid(self.fc2(x))  # Output probabilities for each class
        return x
