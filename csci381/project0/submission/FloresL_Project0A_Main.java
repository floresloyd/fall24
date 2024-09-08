import java.io.BufferedReader;
import java.io.FileReader;
import java.io.BufferedWriter;
import java.io.FileWriter;
import java.io.IOException;

class Person {
    private String name;
    private int age;

    Person(String name, int age){
        this.name = name;
        this.age = age;
    }

    void printPerson(BufferedWriter writer) throws IOException {
        String describe = name + " is " + age + " years old. ";
        writer.write(describe + "\n");
    }
}


/**
 * FloresL_Project0_Main
 */
public class FloresL_Project0A_Main {
    public static void main(String[] args) throws IOException {
        BufferedReader reader = new BufferedReader(new FileReader(args[0])); // input file
        BufferedWriter writer = new BufferedWriter(new FileWriter(args[1])); // output file
        
        String line = reader.readLine();               
        int numOfPeople = Integer.parseInt(line);
        Person people[] = new Person[4];                                    // Instantiate Person array
        writer.write("There are " + numOfPeople + " people \n" );

        int index = 0;
        while (index < numOfPeople){
            line = reader.readLine();
            String data[] = line.split(" ");                          // Split name and age into an array
            String name = data[0];
            int age = Integer.parseInt(data[1]);
            Person p = new Person(name, age);
            people[index++] = p;
        }

        for(Person person : people){
            person.printPerson(writer);
        }
        
        reader.close();
        writer.close();
    }
    
}