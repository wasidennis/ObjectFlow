#ifndef HISTOGRAM_H
#define HISTOGRAM_H

#include <vector>
#include <iostream>

template <class TYPE>
class Histogram {

protected:
    std::vector<TYPE> *hist;
    int num_bins;
    TYPE delta;
    TYPE min;
    TYPE sum;

public:
    Histogram(int bins, int _min, int _max) {
        hist = new std::vector<TYPE>();
        hist->resize(bins, 0);

        num_bins = bins;
        min = _min;
        sum = 0;
        delta = (_max-_min)/bins;
    }

    Histogram(Histogram<TYPE>& copy) {
        sum = copy.sum;
        min = copy.min;
        delta = copy.delta;
        num_bins = copy.num_bins;
        hist = new std::vector<TYPE>(*copy.hist);
    }

    ~Histogram() {
        delete hist;
    }

    int chooseBin(TYPE sample) {
        // shift to the origin, then divide into delta to figure out bin number
        int bin = (sample - min) / delta;
        // Handle samples above or below range
        if (bin < 0) bin = 0;
        if (bin >= num_bins) bin = num_bins-1;

        return bin;
    }

    void addSample(TYPE sample) {
        int bin = chooseBin(sample);
        hist->at(bin)=hist->at(bin)+1;
        sum++;
    }

    void mergeHistogram(Histogram& h) {
        for(int n=0; n<num_bins; n++) {
            hist->at(n) += h.hist->at(n);
            sum += h.hist->at(n);
        }
    }

    double chiSquared(Histogram& h) {
        double chi = 0.0;
        for (int i=0; i<num_bins; i++) {
            // Get bins and normalize them
            double ss = hist->at(i) / double(sum);
            double hs = h.hist->at(i) / double(h.sum);

            // Compute chi-squared
            double a = ss + hs;
            if (a == 0.0) continue;
            double b = ss - hs;
            chi += b*b / a;
        }
        return chi/2.0;
    }

    void print() {
        for (int i = 0; i < num_bins; i++) {
            std::cout << hist->at(i)/double(sum) << " ";
        }
        std::cout << std::endl;
    }
};

#endif
