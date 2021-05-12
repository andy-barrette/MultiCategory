Created by Andrew Barrette

Calculate auto-mapping between a category and itself. The program was originally designed to answer questions such as:
"If F=kx and U=kx^2 suggest x oscillating with respect to t, then p=mv and K=mv^2 suggest v oscillating with respect to what?"

A category is a collection of objects and morphisms between objects. In the example above, {F,k,x} are all objects, and a*b->c is the morphism connecting {k,x} to F. A morphism connects N objects to 1 object.

To start a new project:
1. Create a new folder and drag the MCcat executable into it.
2. Create MC.txt and open it.
3. Define list of objects separated by commas.
4. Newline. Define list of morphisms separated by semicolons. Morphism format is [name],[number of inputs].
5. Newline. List rules, separated by newlines. Rule format is [morphism name],[comma-separated list of inputs]=[output].
6. Save and run MCcat executable.
7. MCcat will create MCmap.txt containing the completed mapping, insofar as it is possible based on the specified rules list. Mappings that cannot be deduced with the given information are labelled a#.