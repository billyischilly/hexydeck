

struct Vowel {
  int frequency[5];
  float gain[5];
  float resonance[5];
};

// Instance of the Vowel struct for the 'a' vowel, soprano
Vowel a_soprano = {
  { 730, 1090, 2440, 2830, 3250 },
  { 1.0, 0.630957, 0.251189, 0.251189, 0.562341 },
  { 3.97, 3.37, 2.6, 2.1, 1.75 }
};

// Instance of the Vowel struct for the 'a' vowel, alto
Vowel a_alto = {
  { 660, 1120, 2650, 2900, 3250 },
  { 1.0, 0.501187, 0.251189, 0.158489, 0.398107 },
  { 3.5, 3.0, 2.5, 2.0, 1.75 }
};

// Instance of the Vowel struct for the 'a' vowel, tenor
Vowel a_tenor = {
  { 660, 1220, 2290, 2830, 3250 },
  { 1.0, 0.630957, 0.158489, 0.251189, 0.398107 },
  { 3.0, 2.5, 2.0, 1.5, 1.25 }
};

// Instance of the Vowel struct for the 'a' vowel, bass
Vowel a_bass = {
  { 530, 730, 1090, 2830, 3250 },
  { 1.0, 0.354813, 0.630957, 0.199526, 0.398107 },
  { 3.0, 2.5, 2.0, 1.5, 1.25 }
};

// Instance of the Vowel struct for the 'e' vowel, soprano
Vowel e_soprano = {
  { 530, 1840, 2480, 2900, 3250 },
  { 1.0, 0.158489, 0.035481, 0.0707946, 0.199526 },
  { 3.0, 3.2, 2.7, 2.1, 1.8 }
};

// Instance of the Vowel struct for the 'e' vowel, alto
Vowel e_alto = {
  { 500, 1740, 2400, 2800, 3250 },
  { 1.0, 0.199526, 0.0446684, 0.0707946, 0.158489 },
  { 3.0, 3.0, 2.5, 2.0, 1.8 }
};

// Instance of the Vowel struct for the 'e' vowel, tenor
Vowel e_tenor = {
  { 500, 2000, 2550, 2900, 3250 },
  { 1.0, 0.199526, 0.0316228, 0.0707946, 0.199526 },
  { 3.0, 3.0, 2.5, 2.0, 1.8 }
};

// Instance of the Vowel struct for the 'e' vowel, bass
Vowel e_bass = {
  { 400, 1600, 2400, 2800, 3250 },
  { 1.0, 0.199526, 0.035481, 0.0707946, 0.199526 },
  { 3.0, 3.0, 2.5, 2.0, 1.8 }
};

// Instance of the Vowel struct for the 'i' vowel, soprano
Vowel i_soprano = {
  { 330, 1840, 2480, 2900, 3250 },
  { 1.0, 0.1, 0.0707946, 0.112202, 0.199526 },
  { 3.0, 3.2, 2.7, 2.1, 1.8 }
};

// Instance of the Vowel struct for the 'i' vowel, alto
Vowel i_alto = {
  { 400, 1710, 2400, 2800, 3250 },
  { 1.0, 0.125893, 0.0446684, 0.0707946, 0.158489 },
  { 3.0, 3.0, 2.5, 2.0, 1.8 }
};

// Instance of the Vowel struct for the 'i' vowel, tenor
Vowel i_tenor = {
  { 400, 2000, 2550, 2900, 3250 },
  { 1.0, 0.158489, 0.0316228, 0.0630957, 0.199526 },
  { 3.0, 3.0, 2.5, 2.0, 1.8 }
};

// Instance of the Vowel struct for the 'i' vowel, bass
Vowel i_bass = {
  { 240, 2400, 2400, 2800, 3250 },
  { 1.0, 0.125893, 0.035481, 0.0707946, 0.199526 },
  { 3.0, 3.0, 2.5, 2.0, 1.8 }
};

// Instance of the Vowel struct for the 'o' vowel, soprano
Vowel o_soprano = {
  { 450, 800, 2830, 3250, 3250 },
  { 1.0, 0.354813, 0.199526, 0.199526, 0.398107 },
  { 2.0, 1.5, 1.0, 1.0, 1.0 }
};

// Instance of the Vowel struct for the 'o' vowel, alto
Vowel o_alto = {
  { 450, 800, 2830, 3250, 3250 },
  { 1.0, 0.354813, 0.251189, 0.199526, 0.398107 },
  { 2.0, 1.5, 1.0, 1.0, 1.0 }
};

// Instance of the Vowel struct for the 'o' vowel, tenor
Vowel o_tenor = {
  { 450, 800, 2830, 3250, 3250 },
  { 1.0, 0.251189, 0.158489, 0.125893, 0.251189 },
  { 2.0, 1.5, 1.0, 1.0, 1.0 }
};

// Instance of the Vowel struct for the 'o' vowel, bass
Vowel o_bass = {
  { 250, 700, 2830, 3250, 3250 },
  { 1.0, 0.199526, 0.158489, 0.125893, 0.251189 },
  { 2.0, 1.5, 1.0, 1.0, 1.0 }
};

// Instance of the Vowel struct for the 'u' vowel, soprano
Vowel u_soprano = {
  { 325, 700, 2530, 3250, 3250 },
  { 1.0, 0.199526, 0.199526, 0.199526, 0.398107 },
  { 2.0, 1.5, 1.0, 1.0, 1.0 }
};

// Instance of the Vowel struct for the 'u' vowel, alto
Vowel u_alto = {
  { 325, 700, 2530, 3250, 3250 },
  { 1.0, 0.199526, 0.199526, 0.158489, 0.398107 },
  { 2.0, 1.5, 1.0, 1.0, 1.0 }
};

// Instance of the Vowel struct for the 'u' vowel, tenor
Vowel u_tenor = {
  { 325, 700, 2530, 3250, 3250 },
  { 1.0, 0.199526, 0.158489, 0.125893, 0.251189 },
  { 2.0, 1.5, 1.0, 1.0, 1.0 }
};

// Instance of the Vowel struct for the 'u' vowel, bass
Vowel u_bass = {
  { 250, 595, 2530, 3250, 3250 },
  { 1.0, 0.158489, 0.125893, 0.0999999, 0.199526 },
  { 2.0, 1.5, 1.0, 1.0, 1.0 }
};

