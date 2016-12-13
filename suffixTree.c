// A C program to implement Ukkonen's Suffix Tree Construction
#include <stdio.h>
#include <iostream>
#include <string.h>
#include <stdlib.h>
#include <map>
#include <vector>

#include "suffixTree.h"



 
char text[1024 * 1024 * 1024];
 
 //Input string
Node *root = NULL; //Pointer to root node
 
/*lastNewNode will point to newly created internal node,
  waiting for it's suffix link to be set, which might get
  a new suffix link (other than root) in next extension of
  same phase. lastNewNode will be set to NULL when last
  newly created internal node (if there is any) got it's
  suffix link reset to new internal node created in next
  extension of same phase. */
Node *lastNewNode = NULL;
Node *activeNode = NULL;
 
/*activeEdge is represeted as input string character
  index (not the character itself)*/
int activeEdge = -1;
int activeLength = 0;
 
// remainingSuffixCount tells how many suffixes yet to
// be added in tree
int remainingSuffixCount = 0;
int leafEnd = -1;
int *rootEnd = NULL;
int *splitEnd = NULL;
int size = -1; //Length of input string
int memorySize = 0;
 
Node *newNode(int start, int *end)
{
memorySize++;
    Node *node =(Node*) malloc(sizeof(Node));
	if(node == NULL)
	{
		return NULL;
	}
    int i;
    for (i = 0; i < MAX_CHAR; i++)
          node->children[i] = NULL;
 
    /*For root node, suffixLink will be set to NULL
    For internal nodes, suffixLink will be set to root
    by default in  current extension and may change in
    next extension*/
    node->suffixLink = root;
    node->start = start;
    node->end = end;
 
    /*suffixIndex will be set to -1 by default and
      actual suffix index will be set later for leaves
      at the end of all phases*/
    node->suffixIndex = -1;
    return node;
}


 
int edgeLength(Node *n) {
    return *(n->end) - (n->start) + 1;
}
 
int walkDown(Node *currNode)
{
    /*activePoint change for walk down (APCFWD) using
     Skip/Count Trick  (Trick 1). If activeLength is greater
     than current edge length, set next  internal node as
     activeNode and adjust activeEdge and activeLength
     accordingly to represent same activePoint*/
    if (activeLength >= edgeLength(currNode))
    {
        activeEdge += edgeLength(currNode);
        activeLength -= edgeLength(currNode);
        activeNode = currNode;
        return 1;
    }
    return 0;
}
 
void extendSuffixTree(int pos)
{
    /*Extension Rule 1, this takes care of extending all
    leaves created so far in tree*/
    leafEnd = pos;
 
    /*Increment remainingSuffixCount indicating that a
    new suffix added to the list of suffixes yet to be
    added in tree*/
    remainingSuffixCount++;
 
    /*set lastNewNode to NULL while starting a new phase,
     indicating there is no internal node waiting for
     it's suffix link reset in current phase*/
    lastNewNode = NULL;
 
    //Add all suffixes (yet to be added) one by one in tree
    while(remainingSuffixCount > 0) {
 
        if (activeLength == 0)
            activeEdge = pos; //APCFALZ
 
        // There is no outgoing edge starting with
        // activeEdge from activeNode
        if (activeNode->children[text[activeEdge]] == NULL)
        {
            //Extension Rule 2 (A new leaf edge gets created)
            activeNode->children[text[activeEdge]] =
                                          newNode(pos, &leafEnd);
 
            /*A new leaf edge is created in above line starting
             from  an existng node (the current activeNode), and
             if there is any internal node waiting for it's suffix
             link get reset, point the suffix link from that last
             internal node to current activeNode. Then set lastNewNode
             to NULL indicating no more node waiting for suffix link
             reset.*/
            if (lastNewNode != NULL)
            {
                lastNewNode->suffixLink = activeNode;
                lastNewNode = NULL;
            }
        }
        // There is an outgoing edge starting with activeEdge
        // from activeNode
        else
        {
            // Get the next node at the end of edge starting
            // with activeEdge
            Node *next = activeNode->children[text[activeEdge]];
            if (walkDown(next))//Do walkdown
            {
                //Start from next node (the new activeNode)
                continue;
            }
            /*Extension Rule 3 (current character being processed
              is already on the edge)*/
            if (text[next->start + activeLength] == text[pos])
            {
                //If a newly created node waiting for it's 
                //suffix link to be set, then set suffix link 
                //of that waiting node to curent active node
                if(lastNewNode != NULL && activeNode != root)
                {
                    lastNewNode->suffixLink = activeNode;
                    lastNewNode = NULL;
                }
 
                //APCFER3
                activeLength++;
                /*STOP all further processing in this phase
                and move on to next phase*/
                break;
            }
 
            /*We will be here when activePoint is in middle of
              the edge being traversed and current character
              being processed is not  on the edge (we fall off
              the tree). In this case, we add a new internal node
              and a new leaf edge going out of that new node. This
              is Extension Rule 2, where a new leaf edge and a new
            internal node get created*/
            splitEnd = (int*) malloc(sizeof(int));
            *splitEnd = next->start + activeLength - 1;
 
            //New internal node
            Node *split = newNode(next->start, splitEnd);
            activeNode->children[text[activeEdge]] = split;
 
            //New leaf coming out of new internal node
            split->children[text[pos]] = newNode(pos, &leafEnd);
            next->start += activeLength;
            split->children[text[next->start]] = next;
 
            /*We got a new internal node here. If there is any
              internal node created in last extensions of same
              phase which is still waiting for it's suffix link
              reset, do it now.*/
            if (lastNewNode != NULL)
            {
            /*suffixLink of lastNewNode points to current newly
              created internal node*/
                lastNewNode->suffixLink = split;
            }
 
            /*Make the current newly created internal node waiting
              for it's suffix link reset (which is pointing to root
              at present). If we come across any other internal node
              (existing or newly created) in next extension of same
              phase, when a new leaf edge gets added (i.e. when
              Extension Rule 2 applies is any of the next extension
              of same phase) at that point, suffixLink of this node
              will point to that internal node.*/
            lastNewNode = split;
        }
 
        /* One suffix got added in tree, decrement the count of
          suffixes yet to be added.*/
        remainingSuffixCount--;
        if (activeNode == root && activeLength > 0) //APCFER2C1
        {
            activeLength--;
            activeEdge = pos - remainingSuffixCount + 1;
        }
        else if (activeNode != root) //APCFER2C2
        {
            activeNode = activeNode->suffixLink;
        }
    }
}
 
void print(int i, int j)
{
    int k;
    for (k=i; k<=j; k++)
        printf("%c", text[k]);
}
 
//Print the suffix tree as well along with setting suffix index
//So tree will be printed in DFS manner
//Each edge along with it's suffix index will be printed
void setSuffixIndexByDFS(Node *n, int labelHeight)
{
    if (n == NULL)  return;
 
    if (n->start != -1) //A non-root node
    {
        //Print the label on edge from parent to current node
        //print(n->start, *(n->end));
    }
    int leaf = 1;
    int i;
    for (i = 0; i < MAX_CHAR; i++)
    {
        if (n->children[i] != NULL)
        {
            /*if (leaf == 1 && n->start != -1)
                printf(" [%d]\n", n->suffixIndex);*/
 
            //Current node is not a leaf as it has outgoing
            //edges from it.
            leaf = 0;
            setSuffixIndexByDFS(n->children[i], labelHeight +
                                  edgeLength(n->children[i]));
        }
    }
    if (leaf == 1)
    {
        n->suffixIndex = size - labelHeight;
        //printf(" [%d]\n", n->suffixIndex);
    }
}
 
void freeSuffixTreeByPostOrder(Node *n)
{
    if (n == NULL)
        return;
    int i;
    for (i = 0; i < MAX_CHAR; i++)
    {
        if (n->children[i] != NULL)
        {
            freeSuffixTreeByPostOrder(n->children[i]);
        }
    }
    if (n->suffixIndex == -1)
        free(n->end);
    free(n);
}
 
/*Build the suffix tree and print the edge labels along with
suffixIndex. suffixIndex for leaf edges will be >= 0 and
for non-leaf edges will be -1*/
void buildSuffixTree(char * texta)
{
    strcpy(text, texta);
    size = strlen(text);
    int i;
    rootEnd = (int*) malloc(sizeof(int));
    *rootEnd = - 1;
 
    /*Root is a special node with start and end indices as -1,
    as it has no parent from where an edge comes to root*/
    root = newNode(-1, rootEnd);
 
    activeNode = root; //First activeNode will be root
    for (i=0; i<size; i++)
        extendSuffixTree(i);
    int labelHeight = 0;
    setSuffixIndexByDFS(root, labelHeight);
 
    //Free the dynamically allocated memory
    //freeSuffixTreeByPostOrder(root);
    cout << memorySize;
}

Node* getRootNode()
{
  return root;
}

/**/
 
// driver program to test above functions
/*int main(int argc, char *argv[])
{
//  strcpy(text, "abc"); buildSuffixTree();
//  strcpy(text, "xabxac#");    buildSuffixTree();
//  strcpy(text, "xabxa");  buildSuffixTree();
//  strcpy(text, "xabxa$"); buildSuffixTree();
    strcpy(text, "Volume One  The Empress Dowager, Maria Feodorovna, mother of reigning Tsar Alexander I, is the most powerful woman in the Russian royal court. The novel begins in July 1805 in Saint Petersburg, at a soire given by Anna Pavlovna Schererthe maid of honour and confidante to the dowager Empress Maria Feodorovna. Many of the main characters are introduced as they enter the salon. Pierre (Pyotr Kirilovich) Bezukhov is the illegitimate son of a wealthy count, who is dying after a series of strokes. Pierre is about to become embroiled in a struggle for his inheritance. Educated abroad at his father's expense following his mother's death, Pierre is kindhearted but socially awkward, and finds it difficult to integrate into Petersburg society. It is known to everyone at the soire that Pierre is his father's favorite of all the old counts illegitimate progeny.  Also attending the soire is Pierre's friend, Prince Andrei Nikolayevich Bolkonsky, husband of Lise, a charming society favourite. He is disillusioned with Petersburg society and with married life after discovering his wife is empty and superficial, and decides to escape to become aide-de-camp to Prince Mikhail Ilarionovich Kutuzov in the coming war against Napoleon.  The plot moves to Moscow, Russia's former capital, contrasting its provincial, more Russian ways to the more European society of Petersburg. The Rostov family are introduced. Count Ilya Andreyevich Rostov and Countess Natalya Rostova are an affectionate couple but forever worried about their disordered finances. They have four children. Thirteen-year-old Natasha (Natalia Ilyinichna) believes herself in love with Boris Drubetskoy, a young man who is about to join the army as an officer. Twenty-year-old Nikolai Ilyich pledges his love to Sonya (Sofia Alexandrovna), his fifteen-year-old cousin, an orphan who has been brought up by the Rostovs. The eldest child, Vera Ilyinichna, is cold and somewhat haughty but has a good prospective marriage in a Russian-German officer, Adolf Karlovich Berg. Petya (Pyotr Ilyich) at nine is the youngest; like his brother, he is impetuous and eager to join the army when of age.  At Bleak Hills, the Bolkonskys' country estate, Prince Andrei departs for war and leaves his terrified, pregnant wife Lise with his eccentric father Prince Nikolai Andreyevich and devoutly religious sister Maria Nikolayevna Bolkonskaya, who refuses to marry the son of a wealthy aristocrat on account of her devotion to her father.  The second part opens with descriptions of the impending Russian-French war preparations. At the Schngrabern engagement, Nikolai Rostov, now an ensign in the Hussars, has his first taste of battle. Boris Drubetskoy introduces him to Prince Andrei, whom Rostov insults in a fit of impetuousness. He is deeply attracted by Tsar Alexander's charisma. Nikolai gambles and socializes with his officer, Vasily Dmitrich Denisov, and befriends the ruthless, and perhaps, psychopathic Fyodor Ivanovich Dolokhov. Bolkonsky, Rostov and Denisov are involved in the disastrous Battle of Austerlitz, in which Andrei is wounded as he attempts to rescue a Russian standard.  The Battle of Austerlitz is a major event in the book. As the battle is about to start, Prince Andrei thinks the approaching day  be his Toulon, or his Arcola, references to Napoleon's early victories. Later in the battle, however, Andrei falls into enemy hands and even meets his hero, Napoleon. But his previous enthusiasm has been shattered. he no longer thinks much of Napoleon, so petty did his hero with his paltry vanity and delight in victory appear, compared to that lofty, righteous and kindly sky which he had seen and comprehended. Tolstoy portrays Austerlitz as an early test for Russia, one which ended badly because the soldiers fought for irrelevant things like glory or renown rather than the higher virtues which would produce, according to Tolstoy, a victory at Borodino during the 1812 invasion.  Volume Two Book Two begins with Nikolai Rostov briefly returning on leave to Moscow accompanied by his friend Denisov, his officer from his Pavlograd Regiment. He spends an eventful winter at home. Natasha has blossomed into a beautiful young girl. Denisov falls in love with her, proposes marriage but is rejected. Although his mother pleads with Nikolai to marry a wealthy heiress to rescue the family from its dire financial straits, Nikolai refuses. Instead he promises to marry his childhood sweetheart and orphaned cousin, the dowry-less Sonya.  Pierre Bezukhov, upon finally receiving his massive inheritance, is suddenly transformed from a bumbling young man into the most eligible bachelor in the Russian Empire. Despite knowing that it is wrong, he is convinced into marriage with Prince Kuragin's beautiful and immoral daughter Hlne (Elena Vasilyevna Kuragina). Hlne, who is rumoured to be involved in an incestuous affair with her brother Anatol, tells Pierre that she will never have children with him. Hlne is also rumoured to have an affair with Dolokhov, who mocks Pierre in public. Pierre loses his temper and challenges Dolokhov to a duel. Unexpectedly (because Dolokhov is a seasoned dueller), Pierre wounds Dolokhov. Hlne denies her affair, but Pierre is convinced of her guilt and leaves her. In his moral and spiritual confusion, Pierre joins the Freemasons. Much of Book Two concerns his struggles with his passions and his spiritual conflicts. He abandons his former carefree behavior and enters upon a philosophical quest particular to Tolstoy: how should one live a moral life in an ethically imperfect world? The question continually baffles Pierre. He attempts to liberate his serfs, but ultimately achieves nothing of note.  Pierre is contrasted with Prince Andrei Bolkonsky. Andrei recovers from his near-fatal wound in a military hospital and returns home, only to find his wife Lise dying in childbirth. He is stricken by his guilty conscience for not treating her better. His child, Nikolenka, survives.  Burdened with nihilistic disillusionment, Prince Andrei does not return to the army but remains on his estate, working on a project that would codify military behavior to solve problems of disorganization responsible for the loss of life on the Russian side. Pierre visits him and brings new questions: where is God in this amoral world? Pierre is interested in panentheism and the possibility of an afterlife.   Scene in Red Square, Moscow, 1801. Oil on canvas by Fedor Yakovlevich Alekseev. Pierre's wife, Hlne, begs him to take her back, and trying to abide by the Freemason laws of forgiveness, he agrees. Hlne establishes herself as an influential hostess in Petersburg society.  Prince Andrei feels impelled to take his newly written military notions to Petersburg, naively expecting to influence either the Emperor himself or those close to him. Young Natasha, also in Petersburg, is caught up in the excitement of her first grand ball, where she meets Prince Andrei and briefly reinvigorates him with her vivacious charm. Andrei believes he has found purpose in life again and, after paying the Rostovs several visits, proposes marriage to Natasha. However, Andrei's father dislikes the Rostovs and opposes the marriage, and he insists the couple wait a year before marrying. Prince Andrei leaves to recuperate from his wounds abroad, leaving Natasha initially distraught. Count Rostov takes her and Sonya to Moscow in order to raise funds for her trousseau.  Natasha visits the Moscow opera, where she meets Hlne and her brother Anatole. Anatole has since married a Polish woman whom he has abandoned in Poland. He is very attracted to Natasha and determined to seduce her, and conspires with his sister to do so. Anatole succeeds in making Natasha believe he loves her, eventually establishing plans to elope. Natasha writes to Princess Maria, Andrei's sister, breaking off her engagement. At the last moment, Sonya discovers her plans to elope and foils them. Devastated, Natasha makes a suicide attempt and is left seriously ill.  Pierre is initially horrified by Natasha's behavior, but realizes he has fallen in love with her. As the Great Comet of 181112 streaks the sky, life appears to begin anew for Pierre. Prince Andrei coldly accepts Natasha's breaking of the engagement. He tells Pierre that his pride will not allow him to renew his proposal.  Volume Three  The Battle of Borodino, fought on September 7, 1812 and involving more than 250,000 troops and 70,000 casualties was a turning point in Napoleon's failed campaign to take Russia. It is vividly depicted through the plot and characters of War and Peace. Painting by Louis-Franois, Baron Lejeune, 1822. With the help of her family, and the stirrings of religious faith, Natasha manages to persevere in Moscow through this dark period. Meanwhile, the whole of Russia is affected by the coming confrontation between Napoleon's troops and the Russian army. Pierre convinces himself through gematria that Napoleon is the Antichrist of the Book of Revelation. Old Prince Bolkonsky dies of a stroke knowing that French marauders are coming for his estate. No organized help from any Russian army seems available to the Bolkonskys, but Nikolai Rostov turns up at their estate in time to help put down an incipient peasant revolt. He finds himself attracted to the distraught Princess Maria.  Back in Moscow, the patriotic Petya joins a crowd in audience of Czar Alexander and manages to snatch a biscuit thrown from the balcony window of the Cathedral of the Assumption by the Czar. He is nearly crushed by the throngs in his effort. Under the influence of the same patriotism, his father finally allows him to enlist.  Napoleon himself is a main character in this section, and the novel presents him in vivid detail, both personally and as both a thinker and would-be strategist. Also described are the well-organized force of over 400,000 French Army (only 140,000 of them actually French-speaking) that marches through the Russian countryside in the late summer and reaches the outskirts of the city of Smolensk. Pierre decides to leave Moscow and go to watch the Battle of Borodino from a vantage point next to a Russian artillery crew. After watching for a time, he begins to join in carrying ammunition. In the midst of the turmoil he experiences first-hand the death and destruction of war; Eugne's artillery continues to pound Russian support columns, while Marshals Ney and Davout set up a crossfire with artillery positioned on the Semyonovskaya heights. The battle becomes a hideous slaughter for both armies and ends in a standoff. The Russians, however, have won a moral victory by standing up to Napoleon's reputedly invincible army. The Russian army withdraws the next day, allowing Napoleon to march on to Moscow. Among the casualties are Anatole Kuragin and Prince Andrei. Anatole loses a leg, and Andrei suffers a grenade wound in the abdomen. Both are reported dead, but their families are in such disarray that no one can be notified.  Volume Four The Rostovs have waited until the last minute to abandon Moscow, even after it is clear that Kutuzov has retreated past Moscow and Muscovites are being given contradictory instructions on how to either flee or fight. Count Rostopchin, the commander in chief of Moscow, is publishing posters, rousing the citizens to put their faith in religious icons, while at the same time urging them to fight with pitchforks if necessary. Before fleeing himself, he gives orders to burn the city. The Rostovs have a difficult time deciding what to take with them, but in the end, Natasha convinces them to load their carts with the wounded and dying from the Battle of Borodino. Unknown to Natasha, Prince Andrei is amongst the wounded.  When Napoleon's Grand Army finally occupies an abandoned and burning Moscow, Pierre takes off on a quixotic mission to assassinate Napoleon. He becomes anonymous in all the chaos, shedding his responsibilities by wearing peasant clothes and shunning his duties and lifestyle. The only people he sees are Natasha and some of her family, as they depart Moscow. Natasha recognizes and smiles at him, and he in turn realizes the full scope of his love for her.  Pierre saves the life of a French officer who fought at Borodino, yet is taken prisoner by the retreating French during his attempted assassination of Napoleon, after saving a woman from being raped by soldiers in the French Army.   Napoleon's retreat from Moscow. Painting by Adolf Northern (18281876) Pierre becomes friends with a fellow prisoner, Platon Karataev, a peasant with a saintly demeanor. In Karataev, Pierre finally finds what he has been seeking: an honest person of integrity, who is utterly without pretense. Pierre discovers meaning in life simply by interacting with him. After witnessing French soldiers sacking Moscow and shooting Russian civilians arbitrarily, Pierre is forced to march with the Grand Army during its disastrous retreat from Moscow in the harsh Russian winter. After months of trial and tribulation  during which the fever-plagued Karataev is shot by the French  Pierre is finally freed by a Russian raiding party, after a small skirmish with the French that sees the young Petya Rostov killed in action.  Meanwhile, Andrei has been taken in and cared for by the Rostovs, fleeing from Moscow to Yaroslavl. He is reunited with Natasha and his sister Maria before the end of the war. Having lost all will to live, he forgives Natasha in a last act before dying.  As the novel draws to a close, Pierre's wife Hlne dies from an overdose of an abortifacient (Tolstoy does not state it explicitly but the euphemism he uses is unambiguous). Pierre is reunited with Natasha, while the victorious Russians rebuild Moscow. Natasha speaks of Prince Andrei's death and Pierre of Karataev's. Both are aware of a growing bond between them in their bereavement. With the help of Princess Maria, Pierre finds love at last and marries Natasha.  Epilogue in two parts First part The first part of the epilogue begins with the wedding of Pierre and Natasha in 1813. Count Rostov dies soon after, leaving his eldest son Nikolai to take charge of the debt-ridden estate. Nikolai finds himself with the task of maintaining the family on the verge of bankruptcy. His abhorrence at the idea of marrying for wealth almost gets in his way, but finally he marries the now-rich Maria Bolkonskaya and in so doing saves his family from financial ruin.  $"); 

    /*buildSuffixTree();
  cout << strlen(text);
//printf("\n%lu Memory size\n", sizeof(Node) * memorySize);
//  strcpy(text, "geeksforgeeks$"); buildSuffixTree();
//  strcpy(text, "THIS IS A TEST TEXT$"); buildSuffixTree();
//  strcpy(text, "AABAACAADAABAAABAA$"); buildSuffixTree();
    return 0;
}*/
