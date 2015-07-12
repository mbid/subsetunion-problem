#include <algorithm>
#include <string>
#include <iostream>
#include <cassert>

#include "subsequences.hpp"

using std::cout;
using std::endl;
using std::size_t;
using std::array;
using std::ostream;


template<class SubsetsIterator>
bool last_subset_causes_counterexample
(
    SubsetsIterator subsets_begin,
    SubsetsIterator subsets_end
)
{
    auto last_it = subsets_end - 1;
    for(auto i = subsets_begin; i != subsets_end; ++i)
    {
        subset_t first_union = unite(*last_it, *i);
        
        for(auto j = subsets_begin; j != subsets_end; ++j)
        {
            for(auto k = j; k != subsets_end; ++k)
            {
                if(j == i && k == last_it) 
                    continue;

                subset_t second_union = unite(*j, *k);
                if(first_union == second_union)
                    return true;
            }
        }
    }

    return false;
}

template<size_t ProblemSize, size_t CurrentIndex = 0>
struct bruteforce_helper 
{
    template<class AllSubsetsIterator>
    static void doit
    (
        AllSubsetsIterator all_begin,
        AllSubsetsIterator all_end,
        array<subset_t, ProblemSize>& currently_chosen_subsets
    )
    {
        for(auto subset_it = all_begin; subset_it != all_end; ++subset_it)
        {
            currently_chosen_subsets[CurrentIndex] = *subset_it;

            if(!last_subset_causes_counterexample(currently_chosen_subsets.begin(), currently_chosen_subsets.begin() + CurrentIndex + 1))
                bruteforce_helper<ProblemSize, CurrentIndex + 1>::doit(subset_it, all_end, currently_chosen_subsets);
        }
    }
};



template<size_t ProblemSize>
struct bruteforce_helper<ProblemSize, ProblemSize>
{
    template<class AllSubsetsIterator>
    static void doit
    (
        AllSubsetsIterator,
        AllSubsetsIterator,
        array<subset_t, ProblemSize>& currently_chosen_subsets
    )
    {
        cout << "counterexample: \n";
        for(subset_t subset : currently_chosen_subsets)
        {
            cout << "  ";
            print_subset(subset, cout);
            cout << "\n";
        }
        cout << "unions: \n";
        for(auto i = currently_chosen_subsets.begin(); i != currently_chosen_subsets.end(); ++i)
        {
            for(auto j = i + 1; j != currently_chosen_subsets.end(); ++j)
            {
                cout << "  ";
                print_subset(unite(*i, *j), cout);
                cout << "\n";
            }
        }
    }
};

template<size_t ProblemSize>
void bruteforce()
{
    static_assert(ProblemSize >= 1, "");

    cout << "brute-forcing problem size " << ProblemSize << "...\n";

    relevant_subsets_t<ProblemSize - 1> subsets = relevant_subsets<ProblemSize - 1>();
    array<subset_t, ProblemSize> currently_chosen_subsets;
    currently_chosen_subsets.fill(0);

    bruteforce_helper<ProblemSize>::doit(subsets.begin(), subsets.end(), currently_chosen_subsets);
}

int main()
{
    //bruteforce<2>();
    //bruteforce<3>();
    //bruteforce<4>();
    //bruteforce<5>();
    //bruteforce<6>();
    //bruteforce<7>();
    bruteforce<8>();
}

