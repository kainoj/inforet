#  Copyright 2018, University of Freiburg,
#  Chair of Algorithms and Data Structures.
#  Author: Claudius Korzen <korzen@cs.uni-freiburg.de>.


# A simple Named Entity Recognition engine using the Viterbi algorithm.
class NamedEntityRecognition:

    def __init__(self):
        # Transition probabilities :: Map<String, Map<String, float>>
        self.trans_probs = {}

        # Word distribution :: Map<String, Map<String, float>>
        self.word_distribution = {}

    def pos_tag(self, sentence):
        """
        Computes the sequence of POS-tags for the given sentence using
        the Viterbi algorithm, as explained in the lecture.

        The sentence is given as an array of words, without punctuation
        (e.g., commas or full stops).

        The transition probabilities and the word distribution for each tag are
        given in two files on the Wiki. For example code to read these files,
        see the methods given below. Note that there are two special POS-tags
        for the beginning and the end of a sentence.

        Returns a list of tuples (word, POS-tag) that defines the POS-tag
        for each word in the given sentence.

        >>> ner = NamedEntityRecognition()
        >>> ner.read_trans_probs_from_file("example-trans-probs.tsv")
        >>> ner.read_word_distrib_from_file("example-word-distrib.tsv")
        >>> ner.pos_tag(["James", "Bond", "is", "an", "agent"])
        [("James", "NNP"), ("Bond", "NNP"), ("is", "VB"),
          ("an", "OTHER"), ("agent", "NN")]
        """

        # List<Tuple<String, String>> pos_tag(List<String> sentence)
        return []

    """
    Recognizes entities in the given sentence.

    As explained in the lecture, you can simply POS-tag the sentence using the
    method pos_tag() above and take each maximal sequence of words tagged with
    "NNP" as a named entity.

    Returns a list of strings that represent the recognized entities.

    TEST CASE:
        ner = NamedEntityRecognition()
        ner.read_trans_probs_from_file("example-trans-probs.tsv")
        ner.read_word_distrib_from_file("example-word-distrib.tsv")
        ner.find_named_entities(["James", "Bond", "is", "an", "agent"])

    RESULT:
        ["James Bond"]
    List<String> find_named_entities(List<String> sentence)
    """

    def read_trans_probs_from_file(self, filename):
        """
        Reads the transition probabilities from the given file.

        The expected format of the file is one transition probability per line,
        in the format "POS-tag<TAB>POS-tag<TAB>probability"

        >>> ans =  {
        ... 'NNP':   { 'NNP': 0.5, 'VB': 0.3, 'END': 0.2 },
        ... 'BEG':   { 'NNP': 0.4, 'NN': 0.4, 'OTHER': 0.2 },
        ... 'NN':    { 'NN': 0.4, 'VB': 0.4, 'END': 0.2 },
        ... 'VB':    { 'NNP': 0.4, 'NN': 0.4, 'OTHER': 0.2 },
        ... 'OTHER': { 'NNP': 0.1, 'NN': 0.5, 'OTHER': 0.4 }
        ... }
        >>> ner = NamedEntityRecognition()
        >>> ner.read_trans_probs_from_file("example-trans-probs.tsv")
        >>> ans == ner.trans_probs
        True
        """
        with open(filename) as f:
            for line in f:
                tag1, tag2, probability = line.strip().split("\t")

                if tag1 not in self.trans_probs:
                    self.trans_probs[tag1] = {}
                self.trans_probs[tag1][tag2] = float(probability)

    def read_word_distrib_from_file(self, filename):
        """
        Reads the word distribution from the given file.

        The expected format of the file is one word per line, in the format
        "word<TAB>POS-tag<TAB>probaility".
        >>> ans = {
        ...    'NNP':   { 'James': 0.5, 'Bond': 0.5 },
        ...    'NN':    { 'agent': 0.9, 'James': 0.1 },
        ...    'VB':    { 'is': 0.8, 'Bond': 0.2 },
        ...    'OTHER': { 'James': 0.2, 'Bond': 0.2, 'an': 0.6 }
        ... }
        >>> ner = NamedEntityRecognition()
        >>> ner.read_word_distrib_from_file("example-word-distrib.tsv")
        >>> ans == ner.word_distribution
        True
        """

        with open(filename) as f:
            for line in f:
                word, tag, probability = line.strip().split("\t")

                if tag not in self.word_distribution:
                    self.word_distribution[tag] = {}
                self.word_distribution[tag][word] = float(probability)


if __name__ == '__main__':
    # Parse the command line arguments.
    # if len(sys.argv) < 2:
    #     print("Usage: python3 sparql_to_sql.py <database.db>")
    #     sys.exit()
    print("TODO: main")
