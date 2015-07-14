For arbitrary n, it is easy to see that
	{1}, {2}, ..., {n}
is an example proofing $\alpha_n \geq n$.
For $n = 1, \dots, 5$, the program doesn't find a counterexample $M_1, \dots, M_{n+1} \subseteq \{0, \dots, n -1\}$ for $\alpha_n > n$, therefore $\alpha_n = 5$ (modulo bugs :) ).

For $n = 6$, the family

  {4, 5}
  {2, 3}
  {1, 3, 5}
  {1, 2, 4}
  {0, 3, 4}
  {0, 2, 5}
  {0, 1}

proofs $\alpha_6 \geq 7$. The program doesn't find a counterexample for $alpha_6 = 7$.

For $n = 7$, the family

  {5, 6}
  {4, 6}
  {3, 6}
  {2, 5}
  {1, 4}
  {1, 2, 3}
  {0, 3}
  {0, 2, 4}
  {0, 1, 5}

proofs $\alpha_7 \ŋeq 9$. The program runs too long to proof $\alpha_7 = 9$. 
