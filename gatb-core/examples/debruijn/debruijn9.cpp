//! [snippet1]

// We include what we need for the test
#include <gatb/gatb_core.hpp>

#undef NDEBUG
#include <cassert>

/********************************************************************************/
/*                          Node neighbors management                           */
/********************************************************************************/
int main (int argc, char* argv[])
{
    // We create the graph with a bank holding one sequence, and use a specific kmer size and nks to 1
    Graph graph = Graph::create (new BankStrings ("AATGC", NULL), "-kmer-size 4 -nks 1");

    // We get an iterator for all nodes of the graph.
    Graph::Iterator<Node> it = graph.iterator<Node> ();

    // We check that we have only two possible nodes
    assert (it.size() == 2);

    // We loop each node.
    for (it.first(); !it.isDone(); it.next())
    {
        // A shortcut.
        Node& current = it.item();

        // We get the ascii representation of the current iterated node
        std::string s = graph.toString (current);

        // We check that it is correct.
        assert (s=="AATG" || s=="ATGC");

        if (s=="AATG")
        {
            // We get the neighbors of this specific current
            Graph::Vector<Node> neighbors = graph.successors<Node> (current);

            // We check that we got only one successor
            assert (neighbors.size() == 1);

            // Another way to check the number of successors
            assert (graph.outdegree(current) == 1);

            // We check this is the correct neighbor
            assert (graph.toString(neighbors[0]) == "ATGC");
        }
    }

    return EXIT_SUCCESS;
}
//! [snippet1]