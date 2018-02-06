#  Copyright 2018, University of Freiburg,
#  Chair of Algorithms and Data Structures.
#  Author: Claudius Korzen <korzen@cs.uni-freiburg.de>.

import sys

import numpy as np


class NamedEntityRecognition:
    """
    A simple Named Entity Recognition engine using the Viterbi algorithm.
    """
    def __init__(self):
        # Transition probabilities :: Map<String, Map<String, float>>
        self.trans_probs = {}

        # Word distribution :: Map<String, Map<String, float>>
        self.word_dist = {}

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
        ... # doctest: +NORMALIZE_WHITESPACE
        [('James', 'NNP'), ('Bond', 'NNP'), ('is', 'VB'),
         ('an', 'OTHER'), ('agent', 'NN')]
        """
        # print("transition probabilities:")
        # print(self.trans_probs)
        # print("word distribution")
        # print(self.word_dist)

        # TODO(I): is sorting really needed?
        # tag_arr :: tag number (in matrix 'p') → tag name
        tag_arr = sorted(list(self.word_dist))

        n = len(tag_arr)
        m = len(sentence)
        p = np.zeros((n, m))

        tags = []

        # Initiation
        word = sentence[0]
        for i in range(0, n):
            tag = tag_arr[i]
            if tag in self.trans_probs["BEG"] and word in self.word_dist[tag]:
                p[i][0] = self.trans_probs["BEG"][tag] * \
                            self.word_dist[tag][word]
            else:
                # TODO(i): smooth
                p[i][0] = 0
        tags.append(tag_arr[np.argmax(p[:, 0])])

        # Algorithm
        # Iterate through words in sentene
        for j in range(1, m):
            word = sentence[j]
            # Interate through current tags
            for i in range(0, n):
                tag = tag_arr[i]
                # Previous probabs - I'll find max over'em
                prev = [0] * n
                # Iterate through previous tags
                for t in range(0, n):
                    if tag in self.trans_probs[tag_arr[t]]:
                        prev[t] = p[t][j-1] * self.trans_probs[tag_arr[t]][tag]
                if word in self.word_dist[tag]:
                    p[i][j] = max(prev) * self.word_dist[tag][word]

            tags.append(tag_arr[np.argmax(p[:, j])])

        # todo smoothibg
        # List<Tuple<String, String>> pos_tag(List<String> sentence
        return list(zip(sentence, tags))

    def find_named_entities(self, sentence):
        """
        Recognizes entities in the given sentence.

        As explained in the lecture, you can simply POS-tag the sentence using
        the method pos_tag() above and take each maximal sequence of words
        tagged with "NNP" as a named entity.

        Returns a list of strings that represent the recognized entities.

        TEST CASE:
        >>> ner = NamedEntityRecognition()
        >>> ner.read_trans_probs_from_file("example-trans-probs.tsv")
        >>> ner.read_word_distrib_from_file("example-word-distrib.tsv")
        >>> ner.find_named_entities(["James", "Bond", "is", "an", "agent"])
        ['James Bond']
        """
        _, tags = zip(*self.pos_tag(sentence))
        tags = list(tags)

        # Handle "NNP" at the very end
        tags.append("FAKETAG")
        ans = []

        # s - stat of "NNP" sequence
        s = 0
        in_seq = False
        for i in range(0, len(tags)):
            if tags[i] == "NNP":
                if not in_seq:
                    s = i
                    in_seq = True
            else:
                if in_seq:
                    ans.append(' '.join(sentence[s: i]))
                in_seq = False
        return ans

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
        >>> ans == ner.word_dist
        True
        """
        with open(filename) as f:
            for line in f:
                word, tag, probability = line.strip().split("\t")

                if tag not in self.word_dist:
                    self.word_dist[tag] = {}
                self.word_dist[tag][word] = float(probability)


if __name__ == '__main__':
    # Parse the command line arguments.
    if len(sys.argv) < 3:
        print("Usage: python3 viterbi.py <TODO> <TODO>")
        sys.exit()
    print("TODO: main")
