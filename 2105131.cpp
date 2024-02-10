#include <bits/stdc++.h>

using namespace std;

#define gap "   "
enum class CollisionStrategy
{
    SEPERATE_CHAINING,
    DOUBLE_HASHING,
    CUSTOM_PROBING
};

vector<string> insertedKeys(20000);

string generateRandomKey()
{
    string chars = "abcdefghijklmnopqrstuvwxyz";
    string key;
    int len = rand() % 5 + 5; // Random key length between 5 and 10
    for (int i = 0; i < len; ++i)
    {
        key += chars[rand() % chars.size()];
    }
    return key;
}

int hash_function(const std::string &key, int size)
{
    int hash = 0;
    for (char c : key)
    {
        hash = (hash * 37 + c);
    }
    hash = hash % size;
    if (hash < 0)
        hash = hash + size;
    return hash;
}

int hash_function2(const string &key, int size)
{
    int hash = 0;
    hash = key[0] + 27 * key[1] + 27 * 27 * key[2];
    hash = hash % size;
    if (hash < 0)
        hash = hash + size;
    return hash;
}

int auxHash(int key)
{
    return 31 - (key % 31);
}

// Hash table implementation with separate chaining
class HashTable
{
private:
    vector<list<pair<string, int>>> table;
    enum CollisionStrategy collision_strategy;
    int maxChainLength;
    int insertionNumber;
    int deletionNumber;
    int currentMaxChainLength;
    int currectMaxChainIndex;
    int elements;
    int hashF;

public:
    int collisions;
    int initialSize;
    int size;
    HashTable(int table_size, int maxChain, enum CollisionStrategy c, int hashFunc) : size(table_size), initialSize(table_size), table(table_size), maxChainLength(maxChain), collision_strategy(c), insertionNumber(0), deletionNumber(0), currectMaxChainIndex(0), currentMaxChainLength(0), elements(0), collisions(0), hashF(hashFunc) {}

    void reHash(int type = 0)
    {
        printStatsBeforeRehash();
        int newSize;
        if (type == 0)
        {
            newSize = size * 2;
        }
        else
        {
            newSize = size / 2;
        }

        string collisionStaratName = CollisionStrategy::SEPERATE_CHAINING == collision_strategy ? "SEPERATE_CHAINING" : CollisionStrategy::CUSTOM_PROBING == collision_strategy ? "CUSTOM_PROBING"
                                                                                                                                                                                : "DOUBLE_HASHING";
        // cout << "Rehashing..." << newSize << " " <<initialSize<<" is initialsize "<< collisionStaratName << endl;

        vector<list<pair<string, int>>> newTable(newSize);
        // collisions = 0; // eta ki kora lagbe ashole?
        for (int i = 0; i < size; i++)
        {
            // cout << "Rehashing...f" << i << endl;
            for (auto &pair : table[i])
            {
                int index;
                if (hashF == 1)
                {
                    index = hash_function(pair.first, newSize);
                }
                else
                {
                    index = hash_function2(pair.first, newSize);
                }

                // if (newTable[index].size() != 0)
                // {
                //     collisions++;
                // }
                if (collision_strategy == CollisionStrategy::SEPERATE_CHAINING)
                {
                    newTable[index].push_back(pair);
                }
                else if (collision_strategy == CollisionStrategy::CUSTOM_PROBING)
                {
                    int x = 1;
                    // cout<<"Entering custom probing"<<endl;
                    //  int index2 = auxHash(pair.first, newSize);
                    int index2 = auxHash(index);
                    while (newTable[index].size() != 0)
                    {
                        index = (index + i * index2 * 17 + x * x * 31) % newSize;
                        x++;
                    }
                    // cout<<"Exiting custom probing"<<endl;
                    newTable[index].push_back(pair);
                }
                else if (collision_strategy == CollisionStrategy::DOUBLE_HASHING)
                {
                    int x = 1;
                    // int index2 = auxHash(pair.first, newSize);
                    int index2 = auxHash(index);
                    while (newTable[index].size() != 0)
                    {
                        index = (index + 31 * x * index2) % newSize;
                        x++;
                    }
                    newTable[index].push_back(pair);
                }
            }
        }
        table = newTable;
        size = newSize;
        int maxChain = 0;
        for (int i = 0; i < size; i++)
        {
            if (table[i].size() > table[maxChain].size())
            {
                maxChain = i;
                currentMaxChainLength = table[i].size();
                currectMaxChainIndex = i;
            }
        }
        printStatsAfterRehash();
    }

    bool insert(const string &key, int value)
    {
        int index;
        if (hashF == 1)
        {
            index = hash_function(key, size);
        }
        else
        {
            index = hash_function2(key, size);
        }
        if (table[index].size() != 0)
        {
            collisions++;
        }
        if (collision_strategy == CollisionStrategy::SEPERATE_CHAINING)
        {
            // while(table[index].size()!=0){
            //     index = (index+1)%size;
            // }

            // int prevCollision = collisions;
            for (auto &pair : table[index])
            {
                if (pair.first == key)
                {
                    // collisions = prevCollision;
                    return false;
                }
                // collisions++;
            }
            table[index].push_back(make_pair(key, value));
            insertionNumber++;
            // if ((double)(insertionNumber - deletionNumber) / size > 0.8)
            // {
            //     reHash();
            // }
            insertedKeys.push_back(key);
            if (table[index].size() > currentMaxChainLength)
            {
                currentMaxChainLength = table[index].size();
                currectMaxChainIndex = index;
            }
            if (insertionNumber % 100 == 0)
            {
                // cout << currentMaxChainLength << " " << maxChainLength << endl;
                if (currentMaxChainLength > maxChainLength)
                {
                    reHash();
                }
            }
        }
        else if (collision_strategy == CollisionStrategy::CUSTOM_PROBING)
        {
            int i = 1;
            // int index2 = auxHash(key, size);
            int index2 = auxHash(index);
            while (table[index].size() != 0)
            {
                // int prevCollision = collisions;
                if (table[index].front().first == key)
                {
                    // collisions = prevCollision;
                    return false;
                }
                // collisions++;
                index = (index + i * index2 * 17 + i * i * 31) % size;
                i++;
            }
            table[index].push_back(make_pair(key, value));
            insertionNumber++;
            insertedKeys.push_back(key);
            // if ((double)(insertionNumber - deletionNumber) / size > 0.8)
            // {
            //     reHash();
            // }
            if (table[index].size() > currentMaxChainLength)
            {
                currentMaxChainLength = table[index].size();
                currectMaxChainIndex = index;
            }
            if (insertionNumber % 100 == 0)
            {
                // cout << currentMaxChainLength << " " << maxChainLength << endl;
                if (currentMaxChainLength > maxChainLength || (double)(insertionNumber - deletionNumber) / size > 0.5)
                {
                    reHash();
                }
            }
        }
        else if (collision_strategy == CollisionStrategy::DOUBLE_HASHING)
        {
            int i = 1;
            // int index2 = auxHash(key, size);
            int index2 = auxHash(index);
            // int prevCollision = collisions;
            while (table[index].size() != 0)
            {
                if (table[index].front().first == key)
                {
                    // collisions = prevCollision;
                    return false;
                }
                // collisions++;
                index = (index + 31 * i * index2) % size;
                i++;
            }
            table[index].push_back(make_pair(key, value));
            insertionNumber++;
            insertedKeys.push_back(key);
            // if ((double)(insertionNumber - deletionNumber) / size > 0.8)
            // {
            //     reHash();
            // }
            if (table[index].size() > currentMaxChainLength)
            {
                currentMaxChainLength = table[index].size();
                currectMaxChainIndex = index;
            }
            if (insertionNumber % 100 == 0)
            {
                // cout << currentMaxChainLength << " " << maxChainLength << endl;
                if (currentMaxChainLength > maxChainLength || (double)(insertionNumber - deletionNumber) / size > 0.5)
                {
                    reHash();
                }
            }
        }
        return true;
    }

    int search(const string &key)
    {
        int index;
        if (hashF == 1)
        {
            index = hash_function(key, size);
        }
        else
        {
            index = hash_function2(key, size);
        }
        if (collision_strategy == CollisionStrategy::SEPERATE_CHAINING)
        {
            for (auto &pair : table[index])
            {
                if (pair.first == key)
                {
                    return pair.second;
                }
            }
        }
        else if (collision_strategy == CollisionStrategy::CUSTOM_PROBING)
        {
            int i = 1;
            // int index2 = auxHash(key, size);
            int index2 = auxHash(index);
            do
            {
                if (table[index].front().first == key)
                {
                    return table[index].front().second;
                }
                index = (index + i * index2 * 17 + i * i * 31) % size;
                i++;
            } while (table[index].size() != 0);

            if (table[index].front().first == key)
            {
                return table[index].front().second;
            }
        }
        else if (collision_strategy == CollisionStrategy::DOUBLE_HASHING)
        {
            int i = 1;
            // int index2 = auxHash(key, size);
            int index2 = auxHash(index);
            do
            {
                if (table[index].front().first == key)
                {
                    return table[index].front().second;
                }
                index = (index + 31 * i * index2) % size;
                i++;
            } while (table[index].size() != 0);

            if (table[index].front().first == key)
            {
                return table[index].front().second;
            }
        }
        return -1; // Key not found
    }

    void remove(const string &key)
    {
        // cout << "Removing " << key << endl;
        int index;
        if (hashF == 1)
        {
            index = hash_function(key, size);
        }
        else
        {
            index = hash_function2(key, size);
        }
        auto &chain = table[index];
        if (collision_strategy == CollisionStrategy::SEPERATE_CHAINING)
        {
            for (auto it = chain.begin(); it != chain.end(); ++it)
            {
                if (it->first == key)
                {
                    chain.erase(it);
                    // cout << "Deleted " <<key<< " Seperate chaining"<<endl;
                    // cout<<size<<" "<<initialSize<<endl;
                    deletionNumber++;

                    if (deletionNumber % 100 == 0)
                    {
                        int maxChain = 0;
                        for (int i = 0; i < size; i++)
                        {
                            if (table[i].size() > table[maxChain].size())
                            {
                                maxChain = i;
                                currentMaxChainLength = table[i].size();
                                currectMaxChainIndex = i;
                            }
                        }
                        // cout<<table[maxChain].size()<<" "<<maxChainLength<<endl;
                        if (table[maxChain].size() < (double)0.8 * maxChainLength)
                        {
                            // cout<<"i never come here"<<endl;
                            if ((double)size / 2 >= initialSize)
                            {
                                reHash(1);
                                // cout<<"Rehashing done at separate chain"<<endl;
                            }
                        }
                    }
                    return;
                }
            }
        }
        else if (collision_strategy == CollisionStrategy::CUSTOM_PROBING)
        {
            int i = 1;
            // int index2 = auxHash(key, size);
            int index2 = auxHash(index);
            do
            {
                if (table[index].front().first == key)
                {
                    table[index].pop_front();
                    // cout << "Deleted" <<key<< " custom prob"<<endl;
                    deletionNumber++;
                    if (deletionNumber % 100 == 0)
                    {
                        int maxChain = 0;
                        for (int i = 0; i < size; i++)
                        {
                            if (table[i].size() > table[maxChain].size())
                            {
                                maxChain = i;
                                currentMaxChainLength = table[i].size();
                                currectMaxChainIndex = i;
                            }
                        }
                        // if (table[maxChain].size() < 0.8 * maxChainLength)
                        //{
                        // if (size / 2 > initialSize)
                        // {
                        //     reHash(1);
                        //     cout<<"Rehashing done at custom probe"<<endl;
                        // }
                        //}
                    }
                    return;
                }
                index = (index + i * index2 * 17 + i * i * 31) % size;
                i++;
            } while (table[index].size() != 0);
        }
        else if (collision_strategy == CollisionStrategy::DOUBLE_HASHING)
        {
            int i = 1;
            // int index2 = auxHash(key, size);
            int index2 = auxHash(index);
            do
            {
                if (table[index].front().first == key)
                {
                    table[index].pop_front();
                    // cout << "Deleted " <<key<< " double hash"<<endl;
                    deletionNumber++;
                    if (deletionNumber % 100 == 0)
                    {
                        int maxChain = 0;
                        for (int i = 0; i < size; i++)
                        {
                            if (table[i].size() > table[maxChain].size())
                            {
                                maxChain = i;
                                currentMaxChainLength = table[i].size();
                                currectMaxChainIndex = i;
                            }
                        }
                        // if (table[maxChain].size() < 0.8 * maxChainLength)
                        //{
                        // if (size / 2 > initialSize)
                        // {
                        //     reHash(1);
                        //     cout<<"Rehashing done at double hash"<<endl;
                        // }
                        //}
                    }
                    return;
                }
                index = (index + 31 * i * index2) % size;
                i++;
            } while (table[index].size() != 0);
        }
    }

    void printStatsBeforeRehash()
    {
        double avgProbeCount = calculateAverageProbeCount(0.1); // Calculate average probe count for 10% of elements
        cout << "Statistics before rehash:" << endl;
        cout << "Load Factor: " << (double)(insertionNumber - deletionNumber) / size << endl;
        cout << "Maximum Chain Length: " << currentMaxChainLength << " at index " << /*currentMaxChainIndex*/ currectMaxChainIndex << endl;
        cout << "Average Probe Count: " << avgProbeCount << endl;
    }
    void printStatsAfterRehash()
    {
        double avgProbeCount = calculateAverageProbeCount(0.1); // Calculate average probe count for 10% of elements
        cout << "Statistics after rehash:" << endl;
        cout << "Load Factor: " << (double)(insertionNumber - deletionNumber) / size << endl;
        cout << "Maximum Chain Length: " << currentMaxChainLength << " at index " << /*currentMaxChainIndex*/ currectMaxChainIndex << endl;
        cout << "Average Probe Count: " << avgProbeCount << endl;
    }

    double calculateAverageProbeCount(double fraction)
    {
        srand(0); // Seed random number generator
        int totalProbes = 0;
        int numProbes = 0;
        int numElementsToSearch = floor((insertionNumber - deletionNumber) * fraction);
        for (int i = 0; i < numElementsToSearch; ++i)
        {
            string randomKey = generateRandomKey();
            int index;
            if (hashF == 1)
            {
                index = hash_function(randomKey, size);
            }
            else
            {
                index = hash_function2(randomKey, size);
            }
            // int index = hash_function(randomKey, size);

            if (collision_strategy == CollisionStrategy::SEPERATE_CHAINING)
            {
                for (auto &pair : table[index])
                {
                    if (pair.first == randomKey)
                    {
                        numProbes++;

                        break;
                    }
                    numProbes++;
                }
            }
            else if (collision_strategy == CollisionStrategy::CUSTOM_PROBING)
            {
                int i = 1;
                // int index2 = auxHash(randomKey, size);
                int index2 = auxHash(index);
                do
                {
                    if (table[index].front().first == randomKey)
                    {
                        numProbes++;

                        break;
                    }
                    index = (index + i * index2 * 17 + i * i * 31) % size;
                    i++;
                    numProbes++;
                } while (table[index].size() != 0);
            }
            else if (collision_strategy == CollisionStrategy::DOUBLE_HASHING)
            {
                int i = 1;
                // int index2 = auxHash(randomKey, size);
                int index2 = auxHash(index);
                do
                {
                    if (table[index].front().first == randomKey)
                    {
                        numProbes++;

                        break;
                    }
                    index = (index + 31 * i * index2) % size;
                    i++;
                    numProbes++;
                } while (table[index].size() != 0);
            }
            totalProbes += numProbes;

            numProbes = 0;
        }
        return (double)totalProbes / numElementsToSearch;
    }

    void reportGenerate()
    {

        // if (collision_strategy == CollisionStrategy::SEPERATE_CHAINING)
        // {
        //     cout << "SEPERATE_CHAINING" << endl;
        // }
        // else if (collision_strategy == CollisionStrategy::CUSTOM_PROBING)
        // {
        //     cout << "CUSTOM_PROBING" << endl;
        // }
        // else if (collision_strategy == CollisionStrategy::DOUBLE_HASHING)
        // {
        //     cout << "DOUBLE_HASHING" << endl;
        // }
        cout << gap << collisions << gap << gap << gap;
        cout << calculateAverageProbeCount(0.1);

        // while (size > initialSize)
        // {
        //     string word = generateRandomKey();
        //     remove(word);
        // }

        // cout << collisions << endl;
        // cout << calculateAverageProbeCount(0.1) << endl;
    }
};

int main()
{
    freopen("output.txt", "w", stdout);

    // Initialize hash table with size 10007 (a prime number)
    HashTable hash_table(5000, 1000, CollisionStrategy::DOUBLE_HASHING, 1);
    HashTable hash_table2(5000, 1000, CollisionStrategy::CUSTOM_PROBING, 1);
    HashTable hash_table3(5000, 9, CollisionStrategy::SEPERATE_CHAINING, 1);
    HashTable hash_table4(5000, 1000, CollisionStrategy::DOUBLE_HASHING, 2);
    HashTable hash_table5(5000, 1000, CollisionStrategy::CUSTOM_PROBING, 2);
    HashTable hash_table6(5000, 9, CollisionStrategy::SEPERATE_CHAINING, 2);
    HashTable hash_table7(5000, 1000, CollisionStrategy::DOUBLE_HASHING, 1);
    HashTable hash_table8(10000, 1000, CollisionStrategy::CUSTOM_PROBING, 1);
    HashTable hash_table9(10000, 9, CollisionStrategy::SEPERATE_CHAINING, 1);
    HashTable hash_table10(10000, 1000, CollisionStrategy::DOUBLE_HASHING, 2);
    HashTable hash_table11(10000, 1000, CollisionStrategy::CUSTOM_PROBING, 2);
    HashTable hash_table12(10000, 9, CollisionStrategy::SEPERATE_CHAINING, 2);
    HashTable hash_table13(20000, 10, CollisionStrategy::DOUBLE_HASHING, 1);
    HashTable hash_table14(20000, 10, CollisionStrategy::CUSTOM_PROBING, 1);
    HashTable hash_table15(20000, 9, CollisionStrategy::SEPERATE_CHAINING, 1);
    HashTable hash_table16(20000, 10, CollisionStrategy::DOUBLE_HASHING, 2);
    HashTable hash_table17(20000, 10, CollisionStrategy::CUSTOM_PROBING, 2);
    HashTable hash_table18(20000, 9, CollisionStrategy::SEPERATE_CHAINING, 2);
    // Generate and insert 10,000 unique words into the hash table
    int uniqueWords = 0;
    while (uniqueWords < 10000)
    {
        string word = generateRandomKey();
        if (hash_table.search(word) == -1 || hash_table2.search(word) == -1 || hash_table3.search(word) == -1)
        {
            hash_table.insert(word, uniqueWords);   // Insert word with unique ID as value
            hash_table2.insert(word, uniqueWords);  // Insert word with unique ID as value
            hash_table3.insert(word, uniqueWords);  // Insert word with unique ID as value
            hash_table4.insert(word, uniqueWords);  // Insert word with unique ID as value
            hash_table5.insert(word, uniqueWords);  // Insert word with unique ID as value
            hash_table6.insert(word, uniqueWords);  // Insert word with unique ID as value
            hash_table7.insert(word, uniqueWords);  // Insert word with unique ID as value
            hash_table8.insert(word, uniqueWords);  // Insert word with unique ID as value
            hash_table9.insert(word, uniqueWords);  // Insert word with unique ID as value
            hash_table10.insert(word, uniqueWords); // Insert word with unique ID as value
            hash_table11.insert(word, uniqueWords); // Insert word with unique ID as value
            hash_table12.insert(word, uniqueWords); // Insert word with unique ID as value
            hash_table13.insert(word, uniqueWords); // Insert word with unique ID as value
            hash_table14.insert(word, uniqueWords); // Insert word with unique ID as value
            hash_table15.insert(word, uniqueWords); // Insert word with unique ID as value
            hash_table16.insert(word, uniqueWords); // Insert word with unique ID as value
            hash_table17.insert(word, uniqueWords); // Insert word with unique ID as value
            hash_table18.insert(word, uniqueWords); // Insert word with unique ID as value
            uniqueWords++;
        }
        // else
        // {
        //     cout << "Alre" << endl;
        // }
    }

    cout << "Total unique words inserted: " << uniqueWords << endl;

    cout << "(Hash Table  "
         << "(Collision Resolution"
         << "           Hash1"
         << "                            Hash2" << endl;
    cout << "  size)            "
         << "Method)" << gap << "    " << gap << "(#of Collisions)"
         << " "
         << "(Average)" << gap << "(#of Collisions)"
         << "  (Average)" << endl;

    cout << "\n\n                Chaining         ";

    hash_table3.reportGenerate();
    cout << "        ";
    hash_table6.reportGenerate();
    cout << endl;

    cout << "\n   5000      Double Hashing      ";

    hash_table.reportGenerate();
    cout << "        ";
    hash_table4.reportGenerate();
    cout << endl;

    cout << "\n             Custom Probing      ";

    hash_table2.reportGenerate();
    cout << "        ";
    hash_table5.reportGenerate();
    cout << endl;

    cout << "\n\n                Chaining         ";

    hash_table9.reportGenerate();
    cout << "        ";
    hash_table12.reportGenerate();
    cout << endl;

    cout << "\n   10000     Double Hashing      ";

    hash_table7.reportGenerate();
    cout << "        ";
    hash_table10.reportGenerate();
    cout << endl;

    cout << "\n             Custom Probing      ";

    hash_table8.reportGenerate();
    cout << "        ";
    hash_table11.reportGenerate();
    cout << endl;

    cout << "\n\n                Chaining         ";

    hash_table15.reportGenerate();
    cout << "        ";
    hash_table18.reportGenerate();
    cout << endl;

    cout << "\n   20000     Double Hashing      ";

    hash_table13.reportGenerate();
    cout << "        ";
    hash_table16.reportGenerate();
    cout << endl;

    cout << "\n             Custom Probing      ";

    hash_table14.reportGenerate();
    cout << "        ";
    hash_table17.reportGenerate();
    cout << endl;

    // cout << "Hash function 1" << endl;
    // hash_table.reportGenerate();
    // hash_table2.reportGenerate();

    // cout << "Hash function 2" << endl;
    // hash_table4.reportGenerate();
    // hash_table5.reportGenerate();

    // cout << "After Deletion" << endl;

    // cout << hash_table.size << " " << hash_table.initialSize << endl;
    // cout << hash_table2.size << " " << hash_table2.initialSize << endl;
    // cout << hash_table3.size << " " << hash_table3.initialSize << endl;
    // cout << hash_table4.size << " " << hash_table4.initialSize << endl;
    // cout << hash_table5.size << " " << hash_table5.initialSize << endl;
    // cout << hash_table6.size << " " << hash_table6.initialSize << endl;

    int index = 0;
    while (hash_table3.size > hash_table3.initialSize)
    {
        string word = insertedKeys[index];
        hash_table3.remove(word);
        index++;
    }
    index = 0;
    while (hash_table6.size > hash_table6.initialSize)
    {
        string word = insertedKeys[index];
        hash_table6.remove(word);
        index++;
    }
    index = 0;
    while (hash_table9.size > hash_table9.initialSize)
    {
        string word = insertedKeys[index];
        hash_table9.remove(word);
        index++;
    }
    index = 0;
    while (hash_table12.size > hash_table12.initialSize)
    {
        string word = insertedKeys[index];
        hash_table12.remove(word);
        index++;
    }
    index = 0;
    while (hash_table15.size > hash_table15.initialSize)
    {
        string word = insertedKeys[index];
        hash_table15.remove(word);
        index++;
    }
    index = 0;
    while (hash_table18.size > hash_table18.initialSize)
    {
        string word = insertedKeys[index];
        hash_table18.remove(word);
        index++;
    }

    // cout << "Hash function 1" << endl;
    // hash_table.reportGenerate();
    // hash_table2.reportGenerate();
    // hash_table3.reportGenerate();
    // cout << "Hash function 2" << endl;
    // hash_table4.reportGenerate();
    // hash_table5.reportGenerate();
    // hash_table6.reportGenerate();
    // Insert some key-value pairs
    // hash_table3.insert("apple", 5);
    // hash_table3.insert("banana", 10);
    // hash_table3.insert("orange", 7);

    // // Search for a value
    // std::cout << "Value for 'banana': " << hash_table3.search("banana") << std::endl;

    // // Remove a key-value pair
    // hash_table3.remove("apple");

    // // Search for a removed key
    // std::cout << "Value for 'apple': " << hash_table3.search("apple") << std::endl; // Should print -1

    return 0;
}
