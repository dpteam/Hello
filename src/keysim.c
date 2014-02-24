#define UNICODE_MAX_BYTES 4

unsigned short keysymcodes[] = {
 0x003c /*                   leftcaret < LESS-THAN SIGN */
  , 0x003e /*                  rightcaret > GREATER-THAN SIGN */
  , 0x005f /*                    underbar _ LOW LINE */
  , 0x00af /*                     overbar ¯ MACRON */
  , 0x0100 /*                     Amacron Ā LATIN CAPITAL LETTER A WITH MACRON */
  , 0x0101 /*                     amacron ā LATIN SMALL LETTER A WITH MACRON */
  , 0x0102 /*                      Abreve Ă LATIN CAPITAL LETTER A WITH BREVE */
  , 0x0103 /*                      abreve ă LATIN SMALL LETTER A WITH BREVE */
  , 0x0104 /*                             Ą LATIN CAPITAL LETTER A WITH OGONEK */
  , 0x0105 /*                     aogonek ą LATIN SMALL LETTER A WITH OGONEK */
  , 0x0106 /*                      Cacute Ć LATIN CAPITAL LETTER C WITH ACUTE */
  , 0x0107 /*                      cacute ć LATIN SMALL LETTER C WITH ACUTE */
  , 0x0108 /*                 Ccircumflex Ĉ LATIN CAPITAL LETTER C WITH CIRCUMFLEX */
  , 0x0109 /*                 ccircumflex ĉ LATIN SMALL LETTER C WITH CIRCUMFLEX */
  , 0x010a /*                   Cabovedot Ċ LATIN CAPITAL LETTER C WITH DOT ABOVE */
  , 0x010b /*                   cabovedot ċ LATIN SMALL LETTER C WITH DOT ABOVE */
  , 0x010c /*                      Ccaron Č LATIN CAPITAL LETTER C WITH CARON */
  , 0x010d /*                      ccaron č LATIN SMALL LETTER C WITH CARON */
  , 0x010e /*                      Dcaron Ď LATIN CAPITAL LETTER D WITH CARON */
  , 0x010f /*                      dcaron ď LATIN SMALL LETTER D WITH CARON */
  , 0x0110 /*                     Dstroke Đ LATIN CAPITAL LETTER D WITH STROKE */
  , 0x0111 /*                     dstroke đ LATIN SMALL LETTER D WITH STROKE */
  , 0x0112 /*                     Emacron Ē LATIN CAPITAL LETTER E WITH MACRON */
  , 0x0113 /*                     emacron ē LATIN SMALL LETTER E WITH MACRON */
  , 0x0116 /*                   Eabovedot Ė LATIN CAPITAL LETTER E WITH DOT ABOVE */
  , 0x0117 /*                   eabovedot ė LATIN SMALL LETTER E WITH DOT ABOVE */
  , 0x0118 /*                     Eogonek Ę LATIN CAPITAL LETTER E WITH OGONEK */
  , 0x0119 /*                     eogonek ę LATIN SMALL LETTER E WITH OGONEK */
  , 0x011a /*                      Ecaron Ě LATIN CAPITAL LETTER E WITH CARON */
  , 0x011b /*                      ecaron ě LATIN SMALL LETTER E WITH CARON */
  , 0x011c /*                 Gcircumflex Ĝ LATIN CAPITAL LETTER G WITH CIRCUMFLEX */
  , 0x011d /*                 gcircumflex ĝ LATIN SMALL LETTER G WITH CIRCUMFLEX */
  , 0x011e /*                      Gbreve Ğ LATIN CAPITAL LETTER G WITH BREVE */
  , 0x011f /*                      gbreve ğ LATIN SMALL LETTER G WITH BREVE */
  , 0x0120 /*                   Gabovedot Ġ LATIN CAPITAL LETTER G WITH DOT ABOVE */
  , 0x0121 /*                   gabovedot ġ LATIN SMALL LETTER G WITH DOT ABOVE */
  , 0x0122 /*                    Gcedilla Ģ LATIN CAPITAL LETTER G WITH CEDILLA */
  , 0x0123 /*                    gcedilla ģ LATIN SMALL LETTER G WITH CEDILLA */
  , 0x0124 /*                 Hcircumflex Ĥ LATIN CAPITAL LETTER H WITH CIRCUMFLEX */
  , 0x0125 /*                 hcircumflex ĥ LATIN SMALL LETTER H WITH CIRCUMFLEX */
  , 0x0126 /*                     Hstroke Ħ LATIN CAPITAL LETTER H WITH STROKE */
  , 0x0127 /*                     hstroke ħ LATIN SMALL LETTER H WITH STROKE */
  , 0x0128 /*                      Itilde Ĩ LATIN CAPITAL LETTER I WITH TILDE */
  , 0x0129 /*                      itilde ĩ LATIN SMALL LETTER I WITH TILDE */
  , 0x012a /*                     Imacron Ī LATIN CAPITAL LETTER I WITH MACRON */
  , 0x012b /*                     imacron ī LATIN SMALL LETTER I WITH MACRON */
  , 0x012e /*                     Iogonek Į LATIN CAPITAL LETTER I WITH OGONEK */
  , 0x012f /*                     iogonek į LATIN SMALL LETTER I WITH OGONEK */
  , 0x0130 /*                   Iabovedot İ LATIN CAPITAL LETTER I WITH DOT ABOVE */
  , 0x0131 /*                    idotless ı LATIN SMALL LETTER DOTLESS I */
  , 0x0134 /*                 Jcircumflex Ĵ LATIN CAPITAL LETTER J WITH CIRCUMFLEX */
  , 0x0135 /*                 jcircumflex ĵ LATIN SMALL LETTER J WITH CIRCUMFLEX */
  , 0x0136 /*                    Kcedilla Ķ LATIN CAPITAL LETTER K WITH CEDILLA */
  , 0x0137 /*                    kcedilla ķ LATIN SMALL LETTER K WITH CEDILLA */
  , 0x0138 /*                         kra ĸ LATIN SMALL LETTER KRA */
  , 0x0139 /*                      Lacute Ĺ LATIN CAPITAL LETTER L WITH ACUTE */
  , 0x013a /*                      lacute ĺ LATIN SMALL LETTER L WITH ACUTE */
  , 0x013b /*                    Lcedilla Ļ LATIN CAPITAL LETTER L WITH CEDILLA */
  , 0x013c /*                    lcedilla ļ LATIN SMALL LETTER L WITH CEDILLA */
  , 0x013d /*                      Lcaron Ľ LATIN CAPITAL LETTER L WITH CARON */
  , 0x013e /*                      lcaron ľ LATIN SMALL LETTER L WITH CARON */
  , 0x0141 /*                     Lstroke Ł LATIN CAPITAL LETTER L WITH STROKE */
  , 0x0142 /*                     lstroke ł LATIN SMALL LETTER L WITH STROKE */
  , 0x0143 /*                      Nacute Ń LATIN CAPITAL LETTER N WITH ACUTE */
  , 0x0144 /*                      nacute ń LATIN SMALL LETTER N WITH ACUTE */
  , 0x0145 /*                    Ncedilla Ņ LATIN CAPITAL LETTER N WITH CEDILLA */
  , 0x0146 /*                    ncedilla ņ LATIN SMALL LETTER N WITH CEDILLA */
  , 0x0147 /*                      Ncaron Ň LATIN CAPITAL LETTER N WITH CARON */
  , 0x0148 /*                      ncaron ň LATIN SMALL LETTER N WITH CARON */
  , 0x014a /*                         ENG Ŋ LATIN CAPITAL LETTER ENG */
  , 0x014b /*                         eng ŋ LATIN SMALL LETTER ENG */
  , 0x014c /*                     Omacron Ō LATIN CAPITAL LETTER O WITH MACRON */
  , 0x014d /*                     omacron ō LATIN SMALL LETTER O WITH MACRON */
  , 0x0150 /*                Odoubleacute Ő LATIN CAPITAL LETTER O WITH DOUBLE ACUTE */
  , 0x0151 /*                odoubleacute ő LATIN SMALL LETTER O WITH DOUBLE ACUTE */
  , 0x0152 /*                          OE Œ LATIN CAPITAL LIGATURE OE */
  , 0x0153 /*                          oe œ LATIN SMALL LIGATURE OE */
  , 0x0154 /*                      Racute Ŕ LATIN CAPITAL LETTER R WITH ACUTE */
  , 0x0155 /*                      racute ŕ LATIN SMALL LETTER R WITH ACUTE */
  , 0x0156 /*                    Rcedilla Ŗ LATIN CAPITAL LETTER R WITH CEDILLA */
  , 0x0157 /*                    rcedilla ŗ LATIN SMALL LETTER R WITH CEDILLA */
  , 0x0158 /*                      Rcaron Ř LATIN CAPITAL LETTER R WITH CARON */
  , 0x0159 /*                      rcaron ř LATIN SMALL LETTER R WITH CARON */
  , 0x015a /*                      Sacute Ś LATIN CAPITAL LETTER S WITH ACUTE */
  , 0x015b /*                      sacute ś LATIN SMALL LETTER S WITH ACUTE */
  , 0x015c /*                 Scircumflex Ŝ LATIN CAPITAL LETTER S WITH CIRCUMFLEX */
  , 0x015d /*                 scircumflex ŝ LATIN SMALL LETTER S WITH CIRCUMFLEX */
  , 0x015e /*                    Scedilla Ş LATIN CAPITAL LETTER S WITH CEDILLA */
  , 0x015f /*                    scedilla ş LATIN SMALL LETTER S WITH CEDILLA */
  , 0x0160 /*                      Scaron Š LATIN CAPITAL LETTER S WITH CARON */
  , 0x0161 /*                      scaron š LATIN SMALL LETTER S WITH CARON */
  , 0x0162 /*                    Tcedilla Ţ LATIN CAPITAL LETTER T WITH CEDILLA */
  , 0x0163 /*                    tcedilla ţ LATIN SMALL LETTER T WITH CEDILLA */
  , 0x0164 /*                      Tcaron Ť LATIN CAPITAL LETTER T WITH CARON */
  , 0x0165 /*                      tcaron ť LATIN SMALL LETTER T WITH CARON */
  , 0x0166 /*                      Tslash Ŧ LATIN CAPITAL LETTER T WITH STROKE */
  , 0x0167 /*                      tslash ŧ LATIN SMALL LETTER T WITH STROKE */
  , 0x0168 /*                      Utilde Ũ LATIN CAPITAL LETTER U WITH TILDE */
  , 0x0169 /*                      utilde ũ LATIN SMALL LETTER U WITH TILDE */
  , 0x016a /*                     Umacron Ū LATIN CAPITAL LETTER U WITH MACRON */
  , 0x016b /*                     umacron ū LATIN SMALL LETTER U WITH MACRON */
  , 0x016c /*                      Ubreve Ŭ LATIN CAPITAL LETTER U WITH BREVE */
  , 0x016d /*                      ubreve ŭ LATIN SMALL LETTER U WITH BREVE */
  , 0x016e /*                       Uring Ů LATIN CAPITAL LETTER U WITH RING ABOVE */
  , 0x016f /*                       uring ů LATIN SMALL LETTER U WITH RING ABOVE */
  , 0x0170 /*                Udoubleacute Ű LATIN CAPITAL LETTER U WITH DOUBLE ACUTE */
  , 0x0171 /*                udoubleacute ű LATIN SMALL LETTER U WITH DOUBLE ACUTE */
  , 0x0172 /*                     Uogonek Ų LATIN CAPITAL LETTER U WITH OGONEK */
  , 0x0173 /*                     uogonek ų LATIN SMALL LETTER U WITH OGONEK */
  , 0x0178 /*                  Ydiaeresis Ÿ LATIN CAPITAL LETTER Y WITH DIAERESIS */
  , 0x0179 /*                      Zacute Ź LATIN CAPITAL LETTER Z WITH ACUTE */
  , 0x017a /*                      zacute ź LATIN SMALL LETTER Z WITH ACUTE */
  , 0x017b /*                   Zabovedot Ż LATIN CAPITAL LETTER Z WITH DOT ABOVE */
  , 0x017c /*                   zabovedot ż LATIN SMALL LETTER Z WITH DOT ABOVE */
  , 0x017d /*                      Zcaron Ž LATIN CAPITAL LETTER Z WITH CARON */
  , 0x017e /*                      zcaron ž LATIN SMALL LETTER Z WITH CARON */
  , 0x0192 /*                    function ƒ LATIN SMALL LETTER F WITH HOOK */
  , 0x02c7 /*                       caron ˇ CARON */
  , 0x02d8 /*                       breve ˘ BREVE */
  , 0x02d9 /*                    abovedot ˙ DOT ABOVE */
  , 0x02db /*                      ogonek ˛ OGONEK */
  , 0x02dd /*                 doubleacute ˝ DOUBLE ACUTE ACCENT */
  , 0x0385 /*        Greek_accentdieresis ΅ GREEK DIALYTIKA TONOS */
  , 0x0386 /*           Greek_ALPHAaccent Ά GREEK CAPITAL LETTER ALPHA WITH TONOS */
  , 0x0388 /*         Greek_EPSILONaccent Έ GREEK CAPITAL LETTER EPSILON WITH TONOS */
  , 0x0389 /*             Greek_ETAaccent Ή GREEK CAPITAL LETTER ETA WITH TONOS */
  , 0x038a /*            Greek_IOTAaccent Ί GREEK CAPITAL LETTER IOTA WITH TONOS */
  , 0x038c /*         Greek_OMICRONaccent Ό GREEK CAPITAL LETTER OMICRON WITH TONOS */
  , 0x038e /*         Greek_UPSILONaccent Ύ GREEK CAPITAL LETTER UPSILON WITH TONOS */
  , 0x038f /*           Greek_OMEGAaccent Ώ GREEK CAPITAL LETTER OMEGA WITH TONOS */
  , 0x0390 /*    Greek_iotaaccentdieresis ΐ GREEK SMALL LETTER IOTA WITH DIALYTIKA AND TONOS */
  , 0x0391 /*                 Greek_ALPHA Α GREEK CAPITAL LETTER ALPHA */
  , 0x0392 /*                  Greek_BETA Β GREEK CAPITAL LETTER BETA */
  , 0x0393 /*                 Greek_GAMMA Γ GREEK CAPITAL LETTER GAMMA */
  , 0x0394 /*                 Greek_DELTA Δ GREEK CAPITAL LETTER DELTA */
  , 0x0395 /*               Greek_EPSILON Ε GREEK CAPITAL LETTER EPSILON */
  , 0x0396 /*                  Greek_ZETA Ζ GREEK CAPITAL LETTER ZETA */
  , 0x0397 /*                   Greek_ETA Η GREEK CAPITAL LETTER ETA */
  , 0x0398 /*                 Greek_THETA Θ GREEK CAPITAL LETTER THETA */
  , 0x0399 /*                  Greek_IOTA Ι GREEK CAPITAL LETTER IOTA */
  , 0x039a /*                 Greek_KAPPA Κ GREEK CAPITAL LETTER KAPPA */
  , 0x039b /*                Greek_LAMBDA Λ GREEK CAPITAL LETTER LAMDA */
  , 0x039c /*                    Greek_MU Μ GREEK CAPITAL LETTER MU */
  , 0x039d /*                    Greek_NU Ν GREEK CAPITAL LETTER NU */
  , 0x039e /*                    Greek_XI Ξ GREEK CAPITAL LETTER XI */
  , 0x039f /*               Greek_OMICRON Ο GREEK CAPITAL LETTER OMICRON */
  , 0x03a0 /*                    Greek_PI Π GREEK CAPITAL LETTER PI */
  , 0x03a1 /*                   Greek_RHO Ρ GREEK CAPITAL LETTER RHO */
  , 0x03a3 /*                 Greek_SIGMA Σ GREEK CAPITAL LETTER SIGMA */
  , 0x03a4 /*                   Greek_TAU Τ GREEK CAPITAL LETTER TAU */
  , 0x03a5 /*               Greek_UPSILON Υ GREEK CAPITAL LETTER UPSILON */
  , 0x03a6 /*                   Greek_PHI Φ GREEK CAPITAL LETTER PHI */
  , 0x03a7 /*                   Greek_CHI Χ GREEK CAPITAL LETTER CHI */
  , 0x03a8 /*                   Greek_PSI Ψ GREEK CAPITAL LETTER PSI */
  , 0x03a9 /*                 Greek_OMEGA Ω GREEK CAPITAL LETTER OMEGA */
  , 0x03aa /*         Greek_IOTAdiaeresis Ϊ GREEK CAPITAL LETTER IOTA WITH DIALYTIKA */
  , 0x03ab /*       Greek_UPSILONdieresis Ϋ GREEK CAPITAL LETTER UPSILON WITH DIALYTIKA */
  , 0x03ac /*           Greek_alphaaccent ά GREEK SMALL LETTER ALPHA WITH TONOS */
  , 0x03ad /*         Greek_epsilonaccent έ GREEK SMALL LETTER EPSILON WITH TONOS */
  , 0x03ae /*             Greek_etaaccent ή GREEK SMALL LETTER ETA WITH TONOS */
  , 0x03af /*            Greek_iotaaccent ί GREEK SMALL LETTER IOTA WITH TONOS */
  , 0x03b0 /* Greek_upsilonaccentdieresis ΰ GREEK SMALL LETTER UPSILON WITH DIALYTIKA AND TONOS */
  , 0x03b1 /*                 Greek_alpha α GREEK SMALL LETTER ALPHA */
  , 0x03b2 /*                  Greek_beta β GREEK SMALL LETTER BETA */
  , 0x03b3 /*                 Greek_gamma γ GREEK SMALL LETTER GAMMA */
  , 0x03b4 /*                 Greek_delta δ GREEK SMALL LETTER DELTA */
  , 0x03b5 /*               Greek_epsilon ε GREEK SMALL LETTER EPSILON */
  , 0x03b6 /*                  Greek_zeta ζ GREEK SMALL LETTER ZETA */
  , 0x03b7 /*                   Greek_eta η GREEK SMALL LETTER ETA */
  , 0x03b8 /*                 Greek_theta θ GREEK SMALL LETTER THETA */
  , 0x03b9 /*                  Greek_iota ι GREEK SMALL LETTER IOTA */
  , 0x03ba /*                 Greek_kappa κ GREEK SMALL LETTER KAPPA */
  , 0x03bb /*                Greek_lambda λ GREEK SMALL LETTER LAMDA */
  , 0x03bc /*                    Greek_mu μ GREEK SMALL LETTER MU */
  , 0x03bd /*                    Greek_nu ν GREEK SMALL LETTER NU */
  , 0x03be /*                    Greek_xi ξ GREEK SMALL LETTER XI */
  , 0x03bf /*               Greek_omicron ο GREEK SMALL LETTER OMICRON */
  , 0x03c0 /*                    Greek_pi π GREEK SMALL LETTER PI */
  , 0x03c1 /*                   Greek_rho ρ GREEK SMALL LETTER RHO */
  , 0x03c2 /*       Greek_finalsmallsigma ς GREEK SMALL LETTER FINAL SIGMA */
  , 0x03c3 /*                 Greek_sigma σ GREEK SMALL LETTER SIGMA */
  , 0x03c4 /*                   Greek_tau τ GREEK SMALL LETTER TAU */
  , 0x03c5 /*               Greek_upsilon υ GREEK SMALL LETTER UPSILON */
  , 0x03c6 /*                   Greek_phi φ GREEK SMALL LETTER PHI */
  , 0x03c7 /*                   Greek_chi χ GREEK SMALL LETTER CHI */
  , 0x03c8 /*                   Greek_psi ψ GREEK SMALL LETTER PSI */
  , 0x03c9 /*                 Greek_omega ω GREEK SMALL LETTER OMEGA */
  , 0x03ca /*          Greek_iotadieresis ϊ GREEK SMALL LETTER IOTA WITH DIALYTIKA */
  , 0x03cb /*       Greek_upsilondieresis ϋ GREEK SMALL LETTER UPSILON WITH DIALYTIKA */
  , 0x03cc /*         Greek_omicronaccent ό GREEK SMALL LETTER OMICRON WITH TONOS */
  , 0x03cd /*         Greek_upsilonaccent ύ GREEK SMALL LETTER UPSILON WITH TONOS */
  , 0x03ce /*           Greek_omegaaccent ώ GREEK SMALL LETTER OMEGA WITH TONOS */
  , 0x0401 /*                 Cyrillic_IO Ё CYRILLIC CAPITAL LETTER IO */
  , 0x0402 /*                 Serbian_DJE Ђ CYRILLIC CAPITAL LETTER DJE */
  , 0x0403 /*               Macedonia_GJE Ѓ CYRILLIC CAPITAL LETTER GJE */
  , 0x0404 /*                Ukrainian_IE Є CYRILLIC CAPITAL LETTER UKRAINIAN IE */
  , 0x0405 /*               Macedonia_DSE Ѕ CYRILLIC CAPITAL LETTER DZE */
  , 0x0406 /*                 Ukrainian_I І CYRILLIC CAPITAL LETTER BYELORUSSIAN-UKRAINIAN I */
  , 0x0407 /*                Ukrainian_YI Ї CYRILLIC CAPITAL LETTER YI */
  , 0x0408 /*                 Cyrillic_JE Ј CYRILLIC CAPITAL LETTER JE */
  , 0x0409 /*                Cyrillic_LJE Љ CYRILLIC CAPITAL LETTER LJE */
  , 0x040a /*                Cyrillic_NJE Њ CYRILLIC CAPITAL LETTER NJE */
  , 0x040b /*                Serbian_TSHE Ћ CYRILLIC CAPITAL LETTER TSHE */
  , 0x040c /*               Macedonia_KJE Ќ CYRILLIC CAPITAL LETTER KJE */
  , 0x040e /*         Byelorussian_SHORTU Ў CYRILLIC CAPITAL LETTER SHORT U */
  , 0x040f /*               Cyrillic_DZHE Џ CYRILLIC CAPITAL LETTER DZHE */
  , 0x0410 /*                  Cyrillic_A А CYRILLIC CAPITAL LETTER A */
  , 0x0411 /*                 Cyrillic_BE Б CYRILLIC CAPITAL LETTER BE */
  , 0x0412 /*                 Cyrillic_VE В CYRILLIC CAPITAL LETTER VE */
  , 0x0413 /*                Cyrillic_GHE Г CYRILLIC CAPITAL LETTER GHE */
  , 0x0414 /*                 Cyrillic_DE Д CYRILLIC CAPITAL LETTER DE */
  , 0x0415 /*                 Cyrillic_IE Е CYRILLIC CAPITAL LETTER IE */
  , 0x0416 /*                Cyrillic_ZHE Ж CYRILLIC CAPITAL LETTER ZHE */
  , 0x0417 /*                 Cyrillic_ZE З CYRILLIC CAPITAL LETTER ZE */
  , 0x0418 /*                  Cyrillic_I И CYRILLIC CAPITAL LETTER I */
  , 0x0419 /*             Cyrillic_SHORTI Й CYRILLIC CAPITAL LETTER SHORT I */
  , 0x041a /*                 Cyrillic_KA К CYRILLIC CAPITAL LETTER KA */
  , 0x041b /*                 Cyrillic_EL Л CYRILLIC CAPITAL LETTER EL */
  , 0x041c /*                 Cyrillic_EM М CYRILLIC CAPITAL LETTER EM */
  , 0x041d /*                 Cyrillic_EN Н CYRILLIC CAPITAL LETTER EN */
  , 0x041e /*                  Cyrillic_O О CYRILLIC CAPITAL LETTER O */
  , 0x041f /*                 Cyrillic_PE П CYRILLIC CAPITAL LETTER PE */
  , 0x0420 /*                 Cyrillic_ER Р CYRILLIC CAPITAL LETTER ER */
  , 0x0421 /*                 Cyrillic_ES С CYRILLIC CAPITAL LETTER ES */
  , 0x0422 /*                 Cyrillic_TE Т CYRILLIC CAPITAL LETTER TE */
  , 0x0423 /*                  Cyrillic_U У CYRILLIC CAPITAL LETTER U */
  , 0x0424 /*                 Cyrillic_EF Ф CYRILLIC CAPITAL LETTER EF */
  , 0x0425 /*                 Cyrillic_HA Х CYRILLIC CAPITAL LETTER HA */
  , 0x0426 /*                Cyrillic_TSE Ц CYRILLIC CAPITAL LETTER TSE */
  , 0x0427 /*                Cyrillic_CHE Ч CYRILLIC CAPITAL LETTER CHE */
  , 0x0428 /*                Cyrillic_SHA Ш CYRILLIC CAPITAL LETTER SHA */
  , 0x0429 /*              Cyrillic_SHCHA Щ CYRILLIC CAPITAL LETTER SHCHA */
  , 0x042a /*           Cyrillic_HARDSIGN Ъ CYRILLIC CAPITAL LETTER HARD SIGN */
  , 0x042b /*               Cyrillic_YERU Ы CYRILLIC CAPITAL LETTER YERU */
  , 0x042c /*           Cyrillic_SOFTSIGN Ь CYRILLIC CAPITAL LETTER SOFT SIGN */
  , 0x042d /*                  Cyrillic_E Э CYRILLIC CAPITAL LETTER E */
  , 0x042e /*                 Cyrillic_YU Ю CYRILLIC CAPITAL LETTER YU */
  , 0x042f /*                 Cyrillic_YA Я CYRILLIC CAPITAL LETTER YA */
  , 0x0430 /*                  Cyrillic_a а CYRILLIC SMALL LETTER A */
  , 0x0431 /*                 Cyrillic_be б CYRILLIC SMALL LETTER BE */
  , 0x0432 /*                 Cyrillic_ve в CYRILLIC SMALL LETTER VE */
  , 0x0433 /*                Cyrillic_ghe г CYRILLIC SMALL LETTER GHE */
  , 0x0434 /*                 Cyrillic_de д CYRILLIC SMALL LETTER DE */
  , 0x0435 /*                 Cyrillic_ie е CYRILLIC SMALL LETTER IE */
  , 0x0436 /*                Cyrillic_zhe ж CYRILLIC SMALL LETTER ZHE */
  , 0x0437 /*                 Cyrillic_ze з CYRILLIC SMALL LETTER ZE */
  , 0x0438 /*                  Cyrillic_i и CYRILLIC SMALL LETTER I */
  , 0x0439 /*             Cyrillic_shorti й CYRILLIC SMALL LETTER SHORT I */
  , 0x043a /*                 Cyrillic_ka к CYRILLIC SMALL LETTER KA */
  , 0x043b /*                 Cyrillic_el л CYRILLIC SMALL LETTER EL */
  , 0x043c /*                 Cyrillic_em м CYRILLIC SMALL LETTER EM */
  , 0x043d /*                 Cyrillic_en н CYRILLIC SMALL LETTER EN */
  , 0x043e /*                  Cyrillic_o о CYRILLIC SMALL LETTER O */
  , 0x043f /*                 Cyrillic_pe п CYRILLIC SMALL LETTER PE */
  , 0x0440 /*                 Cyrillic_er р CYRILLIC SMALL LETTER ER */
  , 0x0441 /*                 Cyrillic_es с CYRILLIC SMALL LETTER ES */
  , 0x0442 /*                 Cyrillic_te т CYRILLIC SMALL LETTER TE */
  , 0x0443 /*                  Cyrillic_u у CYRILLIC SMALL LETTER U */
  , 0x0444 /*                 Cyrillic_ef ф CYRILLIC SMALL LETTER EF */
  , 0x0445 /*                 Cyrillic_ha х CYRILLIC SMALL LETTER HA */
  , 0x0446 /*                Cyrillic_tse ц CYRILLIC SMALL LETTER TSE */
  , 0x0447 /*                Cyrillic_che ч CYRILLIC SMALL LETTER CHE */
  , 0x0448 /*                Cyrillic_sha ш CYRILLIC SMALL LETTER SHA */
  , 0x0449 /*              Cyrillic_shcha щ CYRILLIC SMALL LETTER SHCHA */
  , 0x044a /*           Cyrillic_hardsign ъ CYRILLIC SMALL LETTER HARD SIGN */
  , 0x044b /*               Cyrillic_yeru ы CYRILLIC SMALL LETTER YERU */
  , 0x044c /*           Cyrillic_softsign ь CYRILLIC SMALL LETTER SOFT SIGN */
  , 0x044d /*                  Cyrillic_e э CYRILLIC SMALL LETTER E */
  , 0x044e /*                 Cyrillic_yu ю CYRILLIC SMALL LETTER YU */
  , 0x044f /*                 Cyrillic_ya я CYRILLIC SMALL LETTER YA */
  , 0x0451 /*                 Cyrillic_io ё CYRILLIC SMALL LETTER IO */
  , 0x0452 /*                 Serbian_dje ђ CYRILLIC SMALL LETTER DJE */
  , 0x0453 /*               Macedonia_gje ѓ CYRILLIC SMALL LETTER GJE */
  , 0x0454 /*                Ukrainian_ie є CYRILLIC SMALL LETTER UKRAINIAN IE */
  , 0x0455 /*               Macedonia_dse ѕ CYRILLIC SMALL LETTER DZE */
  , 0x0456 /*                 Ukrainian_i і CYRILLIC SMALL LETTER BYELORUSSIAN-UKRAINIAN I */
  , 0x0457 /*                Ukrainian_yi ї CYRILLIC SMALL LETTER YI */
  , 0x0458 /*                 Cyrillic_je ј CYRILLIC SMALL LETTER JE */
  , 0x0459 /*                Cyrillic_lje љ CYRILLIC SMALL LETTER LJE */
  , 0x045a /*                Cyrillic_nje њ CYRILLIC SMALL LETTER NJE */
  , 0x045b /*                Serbian_tshe ћ CYRILLIC SMALL LETTER TSHE */
  , 0x045c /*               Macedonia_kje ќ CYRILLIC SMALL LETTER KJE */
  , 0x045e /*         Byelorussian_shortu ў CYRILLIC SMALL LETTER SHORT U */
  , 0x045f /*               Cyrillic_dzhe џ CYRILLIC SMALL LETTER DZHE */
  , 0x05d0 /*                hebrew_aleph א HEBREW LETTER ALEF */
  , 0x05d1 /*                  hebrew_bet ב HEBREW LETTER BET */
  , 0x05d2 /*                hebrew_gimel ג HEBREW LETTER GIMEL */
  , 0x05d3 /*                hebrew_dalet ד HEBREW LETTER DALET */
  , 0x05d4 /*                   hebrew_he ה HEBREW LETTER HE */
  , 0x05d5 /*                  hebrew_waw ו HEBREW LETTER VAV */
  , 0x05d6 /*                 hebrew_zain ז HEBREW LETTER ZAYIN */
  , 0x05d7 /*                 hebrew_chet ח HEBREW LETTER HET */
  , 0x05d8 /*                  hebrew_tet ט HEBREW LETTER TET */
  , 0x05d9 /*                  hebrew_yod י HEBREW LETTER YOD */
  , 0x05da /*            hebrew_finalkaph ך HEBREW LETTER FINAL KAF */
  , 0x05db /*                 hebrew_kaph כ HEBREW LETTER KAF */
  , 0x05dc /*                hebrew_lamed ל HEBREW LETTER LAMED */
  , 0x05dd /*             hebrew_finalmem ם HEBREW LETTER FINAL MEM */
  , 0x05de /*                  hebrew_mem מ HEBREW LETTER MEM */
  , 0x05df /*             hebrew_finalnun ן HEBREW LETTER FINAL NUN */
  , 0x05e0 /*                  hebrew_nun נ HEBREW LETTER NUN */
  , 0x05e1 /*               hebrew_samech ס HEBREW LETTER SAMEKH */
  , 0x05e2 /*                 hebrew_ayin ע HEBREW LETTER AYIN */
  , 0x05e3 /*              hebrew_finalpe ף HEBREW LETTER FINAL PE */
  , 0x05e4 /*                   hebrew_pe פ HEBREW LETTER PE */
  , 0x05e5 /*            hebrew_finalzade ץ HEBREW LETTER FINAL TSADI */
  , 0x05e6 /*                 hebrew_zade צ HEBREW LETTER TSADI */
  , 0x05e7 /*                 hebrew_qoph ק HEBREW LETTER QOF */
  , 0x05e8 /*                 hebrew_resh ר HEBREW LETTER RESH */
  , 0x05e9 /*                 hebrew_shin ש HEBREW LETTER SHIN */
  , 0x05ea /*                  hebrew_taw ת HEBREW LETTER TAV */
  , 0x060c /*                Arabic_comma ، ARABIC COMMA */
  , 0x061b /*            Arabic_semicolon ؛ ARABIC SEMICOLON */
  , 0x061f /*        Arabic_question_mark ؟ ARABIC QUESTION MARK */
  , 0x0621 /*                Arabic_hamza ء ARABIC LETTER HAMZA */
  , 0x0622 /*          Arabic_maddaonalef آ ARABIC LETTER ALEF WITH MADDA ABOVE */
  , 0x0623 /*          Arabic_hamzaonalef أ ARABIC LETTER ALEF WITH HAMZA ABOVE */
  , 0x0624 /*           Arabic_hamzaonwaw ؤ ARABIC LETTER WAW WITH HAMZA ABOVE */
  , 0x0625 /*       Arabic_hamzaunderalef إ ARABIC LETTER ALEF WITH HAMZA BELOW */
  , 0x0626 /*           Arabic_hamzaonyeh ئ ARABIC LETTER YEH WITH HAMZA ABOVE */
  , 0x0627 /*                 Arabic_alef ا ARABIC LETTER ALEF */
  , 0x0628 /*                  Arabic_beh ب ARABIC LETTER BEH */
  , 0x0629 /*           Arabic_tehmarbuta ة ARABIC LETTER TEH MARBUTA */
  , 0x062a /*                  Arabic_teh ت ARABIC LETTER TEH */
  , 0x062b /*                 Arabic_theh ث ARABIC LETTER THEH */
  , 0x062c /*                 Arabic_jeem ج ARABIC LETTER JEEM */
  , 0x062d /*                  Arabic_hah ح ARABIC LETTER HAH */
  , 0x062e /*                 Arabic_khah خ ARABIC LETTER KHAH */
  , 0x062f /*                  Arabic_dal د ARABIC LETTER DAL */
  , 0x0630 /*                 Arabic_thal ذ ARABIC LETTER THAL */
  , 0x0631 /*                   Arabic_ra ر ARABIC LETTER REH */
  , 0x0632 /*                 Arabic_zain ز ARABIC LETTER ZAIN */
  , 0x0633 /*                 Arabic_seen س ARABIC LETTER SEEN */
  , 0x0634 /*                Arabic_sheen ش ARABIC LETTER SHEEN */
  , 0x0635 /*                  Arabic_sad ص ARABIC LETTER SAD */
  , 0x0636 /*                  Arabic_dad ض ARABIC LETTER DAD */
  , 0x0637 /*                  Arabic_tah ط ARABIC LETTER TAH */
  , 0x0638 /*                  Arabic_zah ظ ARABIC LETTER ZAH */
  , 0x0639 /*                  Arabic_ain ع ARABIC LETTER AIN */
  , 0x063a /*                Arabic_ghain غ ARABIC LETTER GHAIN */
  , 0x0640 /*              Arabic_tatweel ـ ARABIC TATWEEL */
  , 0x0641 /*                  Arabic_feh ف ARABIC LETTER FEH */
  , 0x0642 /*                  Arabic_qaf ق ARABIC LETTER QAF */
  , 0x0643 /*                  Arabic_kaf ك ARABIC LETTER KAF */
  , 0x0644 /*                  Arabic_lam ل ARABIC LETTER LAM */
  , 0x0645 /*                 Arabic_meem م ARABIC LETTER MEEM */
  , 0x0646 /*                 Arabic_noon ن ARABIC LETTER NOON */
  , 0x0647 /*                   Arabic_ha ه ARABIC LETTER HEH */
  , 0x0648 /*                  Arabic_waw و ARABIC LETTER WAW */
  , 0x0649 /*          Arabic_alefmaksura ى ARABIC LETTER ALEF MAKSURA */
  , 0x064a /*                  Arabic_yeh ي ARABIC LETTER YEH */
  , 0x064b /*             Arabic_fathatan ً ARABIC FATHATAN */
  , 0x064c /*             Arabic_dammatan ٌ ARABIC DAMMATAN */
  , 0x064d /*             Arabic_kasratan ٍ ARABIC KASRATAN */
  , 0x064e /*                Arabic_fatha َ ARABIC FATHA */
  , 0x064f /*                Arabic_damma ُ ARABIC DAMMA */
  , 0x0650 /*                Arabic_kasra ِ ARABIC KASRA */
  , 0x0651 /*               Arabic_shadda ّ ARABIC SHADDA */
  , 0x0652 /*                Arabic_sukun ْ ARABIC SUKUN */
  , 0x0e01 /*                  Thai_kokai ก THAI CHARACTER KO KAI */
  , 0x0e02 /*                Thai_khokhai ข THAI CHARACTER KHO KHAI */
  , 0x0e03 /*               Thai_khokhuat ฃ THAI CHARACTER KHO KHUAT */
  , 0x0e04 /*               Thai_khokhwai ค THAI CHARACTER KHO KHWAI */
  , 0x0e05 /*                Thai_khokhon ฅ THAI CHARACTER KHO KHON */
  , 0x0e06 /*             Thai_khorakhang ฆ THAI CHARACTER KHO RAKHANG */
  , 0x0e07 /*                 Thai_ngongu ง THAI CHARACTER NGO NGU */
  , 0x0e08 /*                Thai_chochan จ THAI CHARACTER CHO CHAN */
  , 0x0e09 /*               Thai_choching ฉ THAI CHARACTER CHO CHING */
  , 0x0e0a /*               Thai_chochang ช THAI CHARACTER CHO CHANG */
  , 0x0e0b /*                   Thai_soso ซ THAI CHARACTER SO SO */
  , 0x0e0c /*                Thai_chochoe ฌ THAI CHARACTER CHO CHOE */
  , 0x0e0d /*                 Thai_yoying ญ THAI CHARACTER YO YING */
  , 0x0e0e /*                Thai_dochada ฎ THAI CHARACTER DO CHADA */
  , 0x0e0f /*                Thai_topatak ฏ THAI CHARACTER TO PATAK */
  , 0x0e10 /*                Thai_thothan ฐ THAI CHARACTER THO THAN */
  , 0x0e11 /*          Thai_thonangmontho ฑ THAI CHARACTER THO NANGMONTHO */
  , 0x0e12 /*             Thai_thophuthao ฒ THAI CHARACTER THO PHUTHAO */
  , 0x0e13 /*                  Thai_nonen ณ THAI CHARACTER NO NEN */
  , 0x0e14 /*                  Thai_dodek ด THAI CHARACTER DO DEK */
  , 0x0e15 /*                  Thai_totao ต THAI CHARACTER TO TAO */
  , 0x0e16 /*               Thai_thothung ถ THAI CHARACTER THO THUNG */
  , 0x0e17 /*              Thai_thothahan ท THAI CHARACTER THO THAHAN */
  , 0x0e18 /*               Thai_thothong ธ THAI CHARACTER THO THONG */
  , 0x0e19 /*                   Thai_nonu น THAI CHARACTER NO NU */
  , 0x0e1a /*               Thai_bobaimai บ THAI CHARACTER BO BAIMAI */
  , 0x0e1b /*                  Thai_popla ป THAI CHARACTER PO PLA */
  , 0x0e1c /*               Thai_phophung ผ THAI CHARACTER PHO PHUNG */
  , 0x0e1d /*                   Thai_fofa ฝ THAI CHARACTER FO FA */
  , 0x0e1e /*                Thai_phophan พ THAI CHARACTER PHO PHAN */
  , 0x0e1f /*                  Thai_fofan ฟ THAI CHARACTER FO FAN */
  , 0x0e20 /*             Thai_phosamphao ภ THAI CHARACTER PHO SAMPHAO */
  , 0x0e21 /*                   Thai_moma ม THAI CHARACTER MO MA */
  , 0x0e22 /*                  Thai_yoyak ย THAI CHARACTER YO YAK */
  , 0x0e23 /*                  Thai_rorua ร THAI CHARACTER RO RUA */
  , 0x0e24 /*                     Thai_ru ฤ THAI CHARACTER RU */
  , 0x0e25 /*                 Thai_loling ล THAI CHARACTER LO LING */
  , 0x0e26 /*                     Thai_lu ฦ THAI CHARACTER LU */
  , 0x0e27 /*                 Thai_wowaen ว THAI CHARACTER WO WAEN */
  , 0x0e28 /*                 Thai_sosala ศ THAI CHARACTER SO SALA */
  , 0x0e29 /*                 Thai_sorusi ษ THAI CHARACTER SO RUSI */
  , 0x0e2a /*                  Thai_sosua ส THAI CHARACTER SO SUA */
  , 0x0e2b /*                  Thai_hohip ห THAI CHARACTER HO HIP */
  , 0x0e2c /*                Thai_lochula ฬ THAI CHARACTER LO CHULA */
  , 0x0e2d /*                   Thai_oang อ THAI CHARACTER O ANG */
  , 0x0e2e /*               Thai_honokhuk ฮ THAI CHARACTER HO NOKHUK */
  , 0x0e2f /*              Thai_paiyannoi ฯ THAI CHARACTER PAIYANNOI */
  , 0x0e30 /*                  Thai_saraa ะ THAI CHARACTER SARA A */
  , 0x0e31 /*             Thai_maihanakat ั THAI CHARACTER MAI HAN-AKAT */
  , 0x0e32 /*                 Thai_saraaa า THAI CHARACTER SARA AA */
  , 0x0e33 /*                 Thai_saraam ำ THAI CHARACTER SARA AM */
  , 0x0e34 /*                  Thai_sarai ิ THAI CHARACTER SARA I */
  , 0x0e35 /*                 Thai_saraii ี THAI CHARACTER SARA II */
  , 0x0e36 /*                 Thai_saraue ึ THAI CHARACTER SARA UE */
  , 0x0e37 /*                Thai_sarauee ื THAI CHARACTER SARA UEE */
  , 0x0e38 /*                  Thai_sarau ุ THAI CHARACTER SARA U */
  , 0x0e39 /*                 Thai_sarauu ู THAI CHARACTER SARA UU */
  , 0x0e3a /*                Thai_phinthu ฺ THAI CHARACTER PHINTHU */
  , 0x0e3f /*                   Thai_baht ฿ THAI CURRENCY SYMBOL BAHT */
  , 0x0e40 /*                  Thai_sarae เ THAI CHARACTER SARA E */
  , 0x0e41 /*                 Thai_saraae แ THAI CHARACTER SARA AE */
  , 0x0e42 /*                  Thai_sarao โ THAI CHARACTER SARA O */
  , 0x0e43 /*          Thai_saraaimaimuan ใ THAI CHARACTER SARA AI MAIMUAN */
  , 0x0e44 /*         Thai_saraaimaimalai ไ THAI CHARACTER SARA AI MAIMALAI */
  , 0x0e45 /*            Thai_lakkhangyao ๅ THAI CHARACTER LAKKHANGYAO */
  , 0x0e46 /*               Thai_maiyamok ๆ THAI CHARACTER MAIYAMOK */
  , 0x0e47 /*              Thai_maitaikhu ็ THAI CHARACTER MAITAIKHU */
  , 0x0e48 /*                  Thai_maiek ่ THAI CHARACTER MAI EK */
  , 0x0e49 /*                 Thai_maitho ้ THAI CHARACTER MAI THO */
  , 0x0e4a /*                 Thai_maitri ๊ THAI CHARACTER MAI TRI */
  , 0x0e4b /*            Thai_maichattawa ๋ THAI CHARACTER MAI CHATTAWA */
  , 0x0e4c /*            Thai_thanthakhat ์ THAI CHARACTER THANTHAKHAT */
  , 0x0e4d /*               Thai_nikhahit ํ THAI CHARACTER NIKHAHIT */
  , 0x0e50 /*                 Thai_leksun ๐ THAI DIGIT ZERO */
  , 0x0e51 /*                Thai_leknung ๑ THAI DIGIT ONE */
  , 0x0e52 /*                Thai_leksong ๒ THAI DIGIT TWO */
  , 0x0e53 /*                 Thai_leksam ๓ THAI DIGIT THREE */
  , 0x0e54 /*                  Thai_leksi ๔ THAI DIGIT FOUR */
  , 0x0e55 /*                  Thai_lekha ๕ THAI DIGIT FIVE */
  , 0x0e56 /*                 Thai_lekhok ๖ THAI DIGIT SIX */
  , 0x0e57 /*                Thai_lekchet ๗ THAI DIGIT SEVEN */
  , 0x0e58 /*                Thai_lekpaet ๘ THAI DIGIT EIGHT */
  , 0x0e59 /*                 Thai_lekkao ๙ THAI DIGIT NINE */
  , 0x11a8 /*             Hangul_J_Kiyeog ᆨ HANGUL JONGSEONG KIYEOK */
  , 0x11a9 /*        Hangul_J_SsangKiyeog ᆩ HANGUL JONGSEONG SSANGKIYEOK */
  , 0x11aa /*         Hangul_J_KiyeogSios ᆪ HANGUL JONGSEONG KIYEOK-SIOS */
  , 0x11ab /*              Hangul_J_Nieun ᆫ HANGUL JONGSEONG NIEUN */
  , 0x11ac /*         Hangul_J_NieunJieuj ᆬ HANGUL JONGSEONG NIEUN-CIEUC */
  , 0x11ad /*         Hangul_J_NieunHieuh ᆭ HANGUL JONGSEONG NIEUN-HIEUH */
  , 0x11ae /*             Hangul_J_Dikeud ᆮ HANGUL JONGSEONG TIKEUT */
  , 0x11af /*              Hangul_J_Rieul ᆯ HANGUL JONGSEONG RIEUL */
  , 0x11b0 /*        Hangul_J_RieulKiyeog ᆰ HANGUL JONGSEONG RIEUL-KIYEOK */
  , 0x11b1 /*         Hangul_J_RieulMieum ᆱ HANGUL JONGSEONG RIEUL-MIEUM */
  , 0x11b2 /*         Hangul_J_RieulPieub ᆲ HANGUL JONGSEONG RIEUL-PIEUP */
  , 0x11b3 /*          Hangul_J_RieulSios ᆳ HANGUL JONGSEONG RIEUL-SIOS */
  , 0x11b4 /*         Hangul_J_RieulTieut ᆴ HANGUL JONGSEONG RIEUL-THIEUTH */
  , 0x11b5 /*        Hangul_J_RieulPhieuf ᆵ HANGUL JONGSEONG RIEUL-PHIEUPH */
  , 0x11b6 /*         Hangul_J_RieulHieuh ᆶ HANGUL JONGSEONG RIEUL-HIEUH */
  , 0x11b7 /*              Hangul_J_Mieum ᆷ HANGUL JONGSEONG MIEUM */
  , 0x11b8 /*              Hangul_J_Pieub ᆸ HANGUL JONGSEONG PIEUP */
  , 0x11b9 /*          Hangul_J_PieubSios ᆹ HANGUL JONGSEONG PIEUP-SIOS */
  , 0x11ba /*               Hangul_J_Sios ᆺ HANGUL JONGSEONG SIOS */
  , 0x11bb /*          Hangul_J_SsangSios ᆻ HANGUL JONGSEONG SSANGSIOS */
  , 0x11bc /*              Hangul_J_Ieung ᆼ HANGUL JONGSEONG IEUNG */
  , 0x11bd /*              Hangul_J_Jieuj ᆽ HANGUL JONGSEONG CIEUC */
  , 0x11be /*              Hangul_J_Cieuc ᆾ HANGUL JONGSEONG CHIEUCH */
  , 0x11bf /*             Hangul_J_Khieuq ᆿ HANGUL JONGSEONG KHIEUKH */
  , 0x11c0 /*              Hangul_J_Tieut ᇀ HANGUL JONGSEONG THIEUTH */
  , 0x11c1 /*             Hangul_J_Phieuf ᇁ HANGUL JONGSEONG PHIEUPH */
  , 0x11c2 /*              Hangul_J_Hieuh ᇂ HANGUL JONGSEONG HIEUH */
  , 0x11eb /*            Hangul_J_PanSios ᇫ HANGUL JONGSEONG PANSIOS */
  , 0x11f0 /*  Hangul_J_KkogjiDalrinIeung ᇰ HANGUL JONGSEONG YESIEUNG */
  , 0x11f9 /*        Hangul_J_YeorinHieuh ᇹ HANGUL JONGSEONG YEORINHIEUH */
  , 0x2002 /*                     enspace   EN SPACE */
  , 0x2003 /*                     emspace   EM SPACE */
  , 0x2004 /*                    em3space   THREE-PER-EM SPACE */
  , 0x2005 /*                    em4space   FOUR-PER-EM SPACE */
  , 0x2007 /*                  digitspace   FIGURE SPACE */
  , 0x2008 /*                  punctspace   PUNCTUATION SPACE */
  , 0x2009 /*                   thinspace   THIN SPACE */
  , 0x200a /*                   hairspace   HAIR SPACE */
  , 0x2012 /*                     figdash ‒ FIGURE DASH */
  , 0x2013 /*                      endash – EN DASH */
  , 0x2014 /*                      emdash — EM DASH */
  , 0x2015 /*              Greek_horizbar ― HORIZONTAL BAR */
  , 0x2017 /*        hebrew_doublelowline ‗ DOUBLE LOW LINE */
  , 0x2018 /*         leftsinglequotemark ‘ LEFT SINGLE QUOTATION MARK */
  , 0x2019 /*        rightsinglequotemark ’ RIGHT SINGLE QUOTATION MARK */
  , 0x201a /*          singlelowquotemark ‚ SINGLE LOW-9 QUOTATION MARK */
  , 0x201c /*         leftdoublequotemark “ LEFT DOUBLE QUOTATION MARK */
  , 0x201d /*        rightdoublequotemark ” RIGHT DOUBLE QUOTATION MARK */
  , 0x201e /*          doublelowquotemark „ DOUBLE LOW-9 QUOTATION MARK */
  , 0x2020 /*                      dagger † DAGGER */
  , 0x2021 /*                doubledagger ‡ DOUBLE DAGGER */
  , 0x2022 /*          enfilledcircbullet • BULLET */
  , 0x2025 /*             doubbaselinedot ‥ TWO DOT LEADER */
  , 0x2026 /*                    ellipsis … HORIZONTAL ELLIPSIS */
  , 0x2032 /*                     minutes ′ PRIME */
  , 0x2033 /*                     seconds ″ DOUBLE PRIME */
  , 0x2038 /*                       caret ‸ CARET */
  , 0x203e /*                    overline ‾ OVERLINE */
  , 0x20a9 /*                  Korean_Won ₩ WON SIGN */
  , 0x20ac /*                        Euro € EURO SIGN */
  , 0x20ac /*                    EuroSign € EURO SIGN */
  , 0x2105 /*                      careof ℅ CARE OF */
  , 0x2116 /*                  numerosign № NUMERO SIGN */
  , 0x2117 /*         phonographcopyright ℗ SOUND RECORDING COPYRIGHT */
  , 0x211e /*                prescription ℞ PRESCRIPTION TAKE */
  , 0x2122 /*                   trademark ™ TRADE MARK SIGN */
  , 0x2153 /*                    onethird ⅓ VULGAR FRACTION ONE THIRD */
  , 0x2154 /*                   twothirds ⅔ VULGAR FRACTION TWO THIRDS */
  , 0x2155 /*                    onefifth ⅕ VULGAR FRACTION ONE FIFTH */
  , 0x2156 /*                   twofifths ⅖ VULGAR FRACTION TWO FIFTHS */
  , 0x2157 /*                 threefifths ⅗ VULGAR FRACTION THREE FIFTHS */
  , 0x2158 /*                  fourfifths ⅘ VULGAR FRACTION FOUR FIFTHS */
  , 0x2159 /*                    onesixth ⅙ VULGAR FRACTION ONE SIXTH */
  , 0x215a /*                  fivesixths ⅚ VULGAR FRACTION FIVE SIXTHS */
  , 0x215b /*                   oneeighth ⅛ VULGAR FRACTION ONE EIGHTH */
  , 0x215c /*                threeeighths ⅜ VULGAR FRACTION THREE EIGHTHS */
  , 0x215d /*                 fiveeighths ⅝ VULGAR FRACTION FIVE EIGHTHS */
  , 0x215e /*                seveneighths ⅞ VULGAR FRACTION SEVEN EIGHTHS */
  , 0x2190 /*                   leftarrow ← LEFTWARDS ARROW */
  , 0x2191 /*                     uparrow ↑ UPWARDS ARROW */
  , 0x2192 /*                  rightarrow → RIGHTWARDS ARROW */
  , 0x2193 /*                   downarrow ↓ DOWNWARDS ARROW */
  , 0x21d2 /*                     implies ⇒ RIGHTWARDS DOUBLE ARROW */
  , 0x21d4 /*                    ifonlyif ⇔ LEFT RIGHT DOUBLE ARROW */
  , 0x2202 /*           partialderivative ∂ PARTIAL DIFFERENTIAL */
  , 0x2207 /*                       nabla ∇ NABLA */
  , 0x2218 /*                         jot ∘ RING OPERATOR */
  , 0x221a /*                     radical √ SQUARE ROOT */
  , 0x221d /*                   variation ∝ PROPORTIONAL TO */
  , 0x221e /*                    infinity ∞ INFINITY */
  , 0x2227 /*                  logicaland ∧ LOGICAL AND */
  , 0x2227 /*                     upcaret ∧ LOGICAL AND */
  , 0x2228 /*                   downcaret ∨ LOGICAL OR */
  , 0x2228 /*                   logicalor ∨ LOGICAL OR */
  , 0x2229 /*                intersection ∩ INTERSECTION */
  , 0x2229 /*                      upshoe ∩ INTERSECTION */
  , 0x222a /*                    downshoe ∪ UNION */
  , 0x222a /*                       union ∪ UNION */
  , 0x222b /*                    integral ∫ INTEGRAL */
  , 0x2234 /*                   therefore ∴ THEREFORE */
  , 0x223c /*                 approximate ∼ TILDE OPERATOR */
  , 0x2243 /*                similarequal ≃ ASYMPTOTICALLY EQUAL TO */
  , 0x2260 /*                    notequal ≠ NOT EQUAL TO */
  , 0x2261 /*                   identical ≡ IDENTICAL TO */
  , 0x2264 /*               lessthanequal ≤ LESS-THAN OR EQUAL TO */
  , 0x2265 /*            greaterthanequal ≥ GREATER-THAN OR EQUAL TO */
  , 0x2282 /*                  includedin ⊂ SUBSET OF */
  , 0x2282 /*                    leftshoe ⊂ SUBSET OF */
  , 0x2283 /*                    includes ⊃ SUPERSET OF */
  , 0x2283 /*                   rightshoe ⊃ SUPERSET OF */
  , 0x22a2 /*                    lefttack ⊢ RIGHT TACK */
  , 0x22a3 /*                   righttack ⊣ LEFT TACK */
  , 0x22a4 /*                      uptack ⊤ DOWN TACK */
  , 0x22a5 /*                    downtack ⊥ UP TACK */
  , 0x2308 /*                     upstile ⌈ LEFT CEILING */
  , 0x230a /*                   downstile ⌊ LEFT FLOOR */
  , 0x2315 /*           telephonerecorder ⌕ TELEPHONE RECORDER */
  , 0x2320 /*                 topintegral ⌠ TOP HALF INTEGRAL */
  , 0x2321 /*                 botintegral ⌡ BOTTOM HALF INTEGRAL */
  , 0x2329 /*            leftanglebracket 〈 LEFT-POINTING ANGLE BRACKET */
  , 0x232a /*           rightanglebracket 〉 RIGHT-POINTING ANGLE BRACKET */
  , 0x2395 /*                        quad ⎕ APL FUNCTIONAL SYMBOL QUAD */
  , 0x239b /*               topleftparens ⎛ ??? */
  , 0x239d /*               botleftparens ⎝ ??? */
  , 0x239e /*              toprightparens ⎞ ??? */
  , 0x23a0 /*              botrightparens ⎠ ??? */
  , 0x23a1 /*            topleftsqbracket ⎡ ??? */
  , 0x23a3 /*            botleftsqbracket ⎣ ??? */
  , 0x23a4 /*           toprightsqbracket ⎤ ??? */
  , 0x23a6 /*           botrightsqbracket ⎦ ??? */
  , 0x23a8 /*        leftmiddlecurlybrace ⎨ ??? */
  , 0x23ac /*       rightmiddlecurlybrace ⎬ ??? */
  , 0x23b7 /*                 leftradical ⎷ ??? */
  , 0x23ba /*              horizlinescan1 ⎺ HORIZONTAL SCAN LINE-1 (Unicode 3.2 draft) */
  , 0x23bb /*              horizlinescan3 ⎻ HORIZONTAL SCAN LINE-3 (Unicode 3.2 draft) */
  , 0x23bc /*              horizlinescan7 ⎼ HORIZONTAL SCAN LINE-7 (Unicode 3.2 draft) */
  , 0x23bd /*              horizlinescan9 ⎽ HORIZONTAL SCAN LINE-9 (Unicode 3.2 draft) */
  , 0x2409 /*                          ht ␉ SYMBOL FOR HORIZONTAL TABULATION */
  , 0x240a /*                          lf ␊ SYMBOL FOR LINE FEED */
  , 0x240b /*                          vt ␋ SYMBOL FOR VERTICAL TABULATION */
  , 0x240c /*                          ff ␌ SYMBOL FOR FORM FEED */
  , 0x240d /*                          cr ␍ SYMBOL FOR CARRIAGE RETURN */
  , 0x2424 /*                          nl ␤ SYMBOL FOR NEWLINE */
  , 0x2500 /*              horizconnector ─ BOX DRAWINGS LIGHT HORIZONTAL */
  , 0x2500 /*              horizlinescan5 ─ BOX DRAWINGS LIGHT HORIZONTAL */
  , 0x2502 /*                     vertbar │ BOX DRAWINGS LIGHT VERTICAL */
  , 0x2502 /*               vertconnector │ BOX DRAWINGS LIGHT VERTICAL */
  , 0x250c /*              topleftradical ┌ BOX DRAWINGS LIGHT DOWN AND RIGHT */
  , 0x250c /*                upleftcorner ┌ BOX DRAWINGS LIGHT DOWN AND RIGHT */
  , 0x2510 /*               uprightcorner ┐ BOX DRAWINGS LIGHT DOWN AND LEFT */
  , 0x2514 /*               lowleftcorner └ BOX DRAWINGS LIGHT UP AND RIGHT */
  , 0x2518 /*              lowrightcorner ┘ BOX DRAWINGS LIGHT UP AND LEFT */
  , 0x251c /*                       leftt ├ BOX DRAWINGS LIGHT VERTICAL AND RIGHT */
  , 0x2524 /*                      rightt ┤ BOX DRAWINGS LIGHT VERTICAL AND LEFT */
  , 0x252c /*                        topt ┬ BOX DRAWINGS LIGHT DOWN AND HORIZONTAL */
  , 0x2534 /*                        bott ┴ BOX DRAWINGS LIGHT UP AND HORIZONTAL */
  , 0x253c /*               crossinglines ┼ BOX DRAWINGS LIGHT VERTICAL AND HORIZONTAL */
  , 0x2592 /*                checkerboard ▒ MEDIUM SHADE */
  , 0x25aa /*            enfilledsqbullet ▪ BLACK SMALL SQUARE */
  , 0x25ab /*          enopensquarebullet ▫ WHITE SMALL SQUARE */
  , 0x25ac /*            filledrectbullet ▬ BLACK RECTANGLE */
  , 0x25ad /*              openrectbullet ▭ WHITE RECTANGLE */
  , 0x25ae /*                emfilledrect ▮ BLACK VERTICAL RECTANGLE */
  , 0x25af /*             emopenrectangle ▯ WHITE VERTICAL RECTANGLE */
  , 0x25b2 /*           filledtribulletup ▲ BLACK UP-POINTING TRIANGLE */
  , 0x25b3 /*             opentribulletup △ WHITE UP-POINTING TRIANGLE */
  , 0x25b6 /*        filledrighttribullet ▶ BLACK RIGHT-POINTING TRIANGLE */
  , 0x25b7 /*           rightopentriangle ▷ WHITE RIGHT-POINTING TRIANGLE */
  , 0x25bc /*         filledtribulletdown ▼ BLACK DOWN-POINTING TRIANGLE */
  , 0x25bd /*           opentribulletdown ▽ WHITE DOWN-POINTING TRIANGLE */
  , 0x25c0 /*         filledlefttribullet ◀ BLACK LEFT-POINTING TRIANGLE */
  , 0x25c1 /*            leftopentriangle ◁ WHITE LEFT-POINTING TRIANGLE */
  , 0x25c6 /*                soliddiamond ◆ BLACK DIAMOND */
  , 0x25cb /*                      circle ○ WHITE CIRCLE */
  , 0x25cb /*                emopencircle ○ WHITE CIRCLE */
  , 0x25cf /*              emfilledcircle ● BLACK CIRCLE */
  , 0x25e6 /*            enopencircbullet ◦ WHITE BULLET */
  , 0x2606 /*                    openstar ☆ WHITE STAR */
  , 0x260e /*                   telephone ☎ BLACK TELEPHONE */
  , 0x2613 /*               signaturemark ☓ SALTIRE */
  , 0x261c /*                 leftpointer ☜ WHITE LEFT POINTING INDEX */
  , 0x261e /*                rightpointer ☞ WHITE RIGHT POINTING INDEX */
  , 0x2640 /*                femalesymbol ♀ FEMALE SIGN */
  , 0x2642 /*                  malesymbol ♂ MALE SIGN */
  , 0x2663 /*                        club ♣ BLACK CLUB SUIT */
  , 0x2665 /*                       heart ♥ BLACK HEART SUIT */
  , 0x2666 /*                     diamond ♦ BLACK DIAMOND SUIT */
  , 0x266d /*                 musicalflat ♭ MUSIC FLAT SIGN */
  , 0x266f /*                musicalsharp ♯ MUSIC SHARP SIGN */
  , 0x2713 /*                   checkmark ✓ CHECK MARK */
  , 0x2717 /*                 ballotcross ✗ BALLOT X */
  , 0x271d /*                  latincross ✝ LATIN CROSS */
  , 0x2720 /*                maltesecross ✠ MALTESE CROSS */
  , 0x3001 /*                  kana_comma 、 IDEOGRAPHIC COMMA */
  , 0x3002 /*               kana_fullstop 。 IDEOGRAPHIC FULL STOP */
  , 0x300c /*         kana_openingbracket 「 LEFT CORNER BRACKET */
  , 0x300d /*         kana_closingbracket 」 RIGHT CORNER BRACKET */
  , 0x309b /*                 voicedsound ゛ KATAKANA-HIRAGANA VOICED SOUND MARK */
  , 0x309c /*             semivoicedsound ゜ KATAKANA-HIRAGANA SEMI-VOICED SOUND MARK */
  , 0x30a1 /*                      kana_a ァ KATAKANA LETTER SMALL A */
  , 0x30a2 /*                      kana_A ア KATAKANA LETTER A */
  , 0x30a3 /*                      kana_i ィ KATAKANA LETTER SMALL I */
  , 0x30a4 /*                      kana_I イ KATAKANA LETTER I */
  , 0x30a5 /*                      kana_u ゥ KATAKANA LETTER SMALL U */
  , 0x30a6 /*                      kana_U ウ KATAKANA LETTER U */
  , 0x30a7 /*                      kana_e ェ KATAKANA LETTER SMALL E */
  , 0x30a8 /*                      kana_E エ KATAKANA LETTER E */
  , 0x30a9 /*                      kana_o ォ KATAKANA LETTER SMALL O */
  , 0x30aa /*                      kana_O オ KATAKANA LETTER O */
  , 0x30ab /*                     kana_KA カ KATAKANA LETTER KA */
  , 0x30ad /*                     kana_KI キ KATAKANA LETTER KI */
  , 0x30af /*                     kana_KU ク KATAKANA LETTER KU */
  , 0x30b1 /*                     kana_KE ケ KATAKANA LETTER KE */
  , 0x30b3 /*                     kana_KO コ KATAKANA LETTER KO */
  , 0x30b5 /*                     kana_SA サ KATAKANA LETTER SA */
  , 0x30b7 /*                    kana_SHI シ KATAKANA LETTER SI */
  , 0x30b9 /*                     kana_SU ス KATAKANA LETTER SU */
  , 0x30bb /*                     kana_SE セ KATAKANA LETTER SE */
  , 0x30bd /*                     kana_SO ソ KATAKANA LETTER SO */
  , 0x30bf /*                     kana_TA タ KATAKANA LETTER TA */
  , 0x30c1 /*                    kana_CHI チ KATAKANA LETTER TI */
  , 0x30c3 /*                    kana_tsu ッ KATAKANA LETTER SMALL TU */
  , 0x30c4 /*                    kana_TSU ツ KATAKANA LETTER TU */
  , 0x30c6 /*                     kana_TE テ KATAKANA LETTER TE */
  , 0x30c8 /*                     kana_TO ト KATAKANA LETTER TO */
  , 0x30ca /*                     kana_NA ナ KATAKANA LETTER NA */
  , 0x30cb /*                     kana_NI ニ KATAKANA LETTER NI */
  , 0x30cc /*                     kana_NU ヌ KATAKANA LETTER NU */
  , 0x30cd /*                     kana_NE ネ KATAKANA LETTER NE */
  , 0x30ce /*                     kana_NO ノ KATAKANA LETTER NO */
  , 0x30cf /*                     kana_HA ハ KATAKANA LETTER HA */
  , 0x30d2 /*                     kana_HI ヒ KATAKANA LETTER HI */
  , 0x30d5 /*                     kana_FU フ KATAKANA LETTER HU */
  , 0x30d8 /*                     kana_HE ヘ KATAKANA LETTER HE */
  , 0x30db /*                     kana_HO ホ KATAKANA LETTER HO */
  , 0x30de /*                     kana_MA マ KATAKANA LETTER MA */
  , 0x30df /*                     kana_MI ミ KATAKANA LETTER MI */
  , 0x30e0 /*                     kana_MU ム KATAKANA LETTER MU */
  , 0x30e1 /*                     kana_ME メ KATAKANA LETTER ME */
  , 0x30e2 /*                     kana_MO モ KATAKANA LETTER MO */
  , 0x30e3 /*                     kana_ya ャ KATAKANA LETTER SMALL YA */
  , 0x30e4 /*                     kana_YA ヤ KATAKANA LETTER YA */
  , 0x30e5 /*                     kana_yu ュ KATAKANA LETTER SMALL YU */
  , 0x30e6 /*                     kana_YU ユ KATAKANA LETTER YU */
  , 0x30e7 /*                     kana_yo ョ KATAKANA LETTER SMALL YO */
  , 0x30e8 /*                     kana_YO ヨ KATAKANA LETTER YO */
  , 0x30e9 /*                     kana_RA ラ KATAKANA LETTER RA */
  , 0x30ea /*                     kana_RI リ KATAKANA LETTER RI */
  , 0x30eb /*                     kana_RU ル KATAKANA LETTER RU */
  , 0x30ec /*                     kana_RE レ KATAKANA LETTER RE */
  , 0x30ed /*                     kana_RO ロ KATAKANA LETTER RO */
  , 0x30ef /*                     kana_WA ワ KATAKANA LETTER WA */
  , 0x30f2 /*                     kana_WO ヲ KATAKANA LETTER WO */
  , 0x30f3 /*                      kana_N ン KATAKANA LETTER N */
  , 0x30fb /*            kana_conjunctive ・ KATAKANA MIDDLE DOT */
  , 0x30fc /*              prolongedsound ー KATAKANA-HIRAGANA PROLONGED SOUND MARK */
  , 0x3131 /*               Hangul_Kiyeog ㄱ HANGUL LETTER KIYEOK */
  , 0x3132 /*          Hangul_SsangKiyeog ㄲ HANGUL LETTER SSANGKIYEOK */
  , 0x3133 /*           Hangul_KiyeogSios ㄳ HANGUL LETTER KIYEOK-SIOS */
  , 0x3134 /*                Hangul_Nieun ㄴ HANGUL LETTER NIEUN */
  , 0x3135 /*           Hangul_NieunJieuj ㄵ HANGUL LETTER NIEUN-CIEUC */
  , 0x3136 /*           Hangul_NieunHieuh ㄶ HANGUL LETTER NIEUN-HIEUH */
  , 0x3137 /*               Hangul_Dikeud ㄷ HANGUL LETTER TIKEUT */
  , 0x3138 /*          Hangul_SsangDikeud ㄸ HANGUL LETTER SSANGTIKEUT */
  , 0x3139 /*                Hangul_Rieul ㄹ HANGUL LETTER RIEUL */
  , 0x313a /*          Hangul_RieulKiyeog ㄺ HANGUL LETTER RIEUL-KIYEOK */
  , 0x313b /*           Hangul_RieulMieum ㄻ HANGUL LETTER RIEUL-MIEUM */
  , 0x313c /*           Hangul_RieulPieub ㄼ HANGUL LETTER RIEUL-PIEUP */
  , 0x313d /*            Hangul_RieulSios ㄽ HANGUL LETTER RIEUL-SIOS */
  , 0x313e /*           Hangul_RieulTieut ㄾ HANGUL LETTER RIEUL-THIEUTH */
  , 0x313f /*          Hangul_RieulPhieuf ㄿ HANGUL LETTER RIEUL-PHIEUPH */
  , 0x3140 /*           Hangul_RieulHieuh ㅀ HANGUL LETTER RIEUL-HIEUH */
  , 0x3141 /*                Hangul_Mieum ㅁ HANGUL LETTER MIEUM */
  , 0x3142 /*                Hangul_Pieub ㅂ HANGUL LETTER PIEUP */
  , 0x3143 /*           Hangul_SsangPieub ㅃ HANGUL LETTER SSANGPIEUP */
  , 0x3144 /*            Hangul_PieubSios ㅄ HANGUL LETTER PIEUP-SIOS */
  , 0x3145 /*                 Hangul_Sios ㅅ HANGUL LETTER SIOS */
  , 0x3146 /*            Hangul_SsangSios ㅆ HANGUL LETTER SSANGSIOS */
  , 0x3147 /*                Hangul_Ieung ㅇ HANGUL LETTER IEUNG */
  , 0x3148 /*                Hangul_Jieuj ㅈ HANGUL LETTER CIEUC */
  , 0x3149 /*           Hangul_SsangJieuj ㅉ HANGUL LETTER SSANGCIEUC */
  , 0x314a /*                Hangul_Cieuc ㅊ HANGUL LETTER CHIEUCH */
  , 0x314b /*               Hangul_Khieuq ㅋ HANGUL LETTER KHIEUKH */
  , 0x314c /*                Hangul_Tieut ㅌ HANGUL LETTER THIEUTH */
  , 0x314d /*               Hangul_Phieuf ㅍ HANGUL LETTER PHIEUPH */
  , 0x314e /*                Hangul_Hieuh ㅎ HANGUL LETTER HIEUH */
  , 0x314f /*                    Hangul_A ㅏ HANGUL LETTER A */
  , 0x3150 /*                   Hangul_AE ㅐ HANGUL LETTER AE */
  , 0x3151 /*                   Hangul_YA ㅑ HANGUL LETTER YA */
  , 0x3152 /*                  Hangul_YAE ㅒ HANGUL LETTER YAE */
  , 0x3153 /*                   Hangul_EO ㅓ HANGUL LETTER EO */
  , 0x3154 /*                    Hangul_E ㅔ HANGUL LETTER E */
  , 0x3155 /*                  Hangul_YEO ㅕ HANGUL LETTER YEO */
  , 0x3156 /*                   Hangul_YE ㅖ HANGUL LETTER YE */
  , 0x3157 /*                    Hangul_O ㅗ HANGUL LETTER O */
  , 0x3158 /*                   Hangul_WA ㅘ HANGUL LETTER WA */
  , 0x3159 /*                  Hangul_WAE ㅙ HANGUL LETTER WAE */
  , 0x315a /*                   Hangul_OE ㅚ HANGUL LETTER OE */
  , 0x315b /*                   Hangul_YO ㅛ HANGUL LETTER YO */
  , 0x315c /*                    Hangul_U ㅜ HANGUL LETTER U */
  , 0x315d /*                  Hangul_WEO ㅝ HANGUL LETTER WEO */
  , 0x315e /*                   Hangul_WE ㅞ HANGUL LETTER WE */
  , 0x315f /*                   Hangul_WI ㅟ HANGUL LETTER WI */
  , 0x3160 /*                   Hangul_YU ㅠ HANGUL LETTER YU */
  , 0x3161 /*                   Hangul_EU ㅡ HANGUL LETTER EU */
  , 0x3162 /*                   Hangul_YI ㅢ HANGUL LETTER YI */
  , 0x3163 /*                    Hangul_I ㅣ HANGUL LETTER I */
  , 0x316d /*     Hangul_RieulYeorinHieuh ㅭ HANGUL LETTER RIEUL-YEORINHIEUH */
  , 0x3171 /*    Hangul_SunkyeongeumMieum ㅱ HANGUL LETTER KAPYEOUNMIEUM */
  , 0x3178 /*    Hangul_SunkyeongeumPieub ㅸ HANGUL LETTER KAPYEOUNPIEUP */
  , 0x317f /*              Hangul_PanSios ㅿ HANGUL LETTER PANSIOS */
  , 0x3181 /*    Hangul_KkogjiDalrinIeung ㆁ HANGUL LETTER YESIEUNG */
  , 0x3184 /*   Hangul_SunkyeongeumPhieuf ㆄ HANGUL LETTER KAPYEOUNPHIEUPH */
  , 0x3186 /*          Hangul_YeorinHieuh ㆆ HANGUL LETTER YEORINHIEUH */
  , 0x318d /*                Hangul_AraeA ㆍ HANGUL LETTER ARAEA */
  , 0x318e /*               Hangul_AraeAE ㆎ HANGUL LETTER ARAEAE */
};

char keysymletters[][UNICODE_MAX_BYTES] = {
"<",
">",
"_",
"¯",
"Ā",
"ā",
"Ă",
"ă",
"Ą",
"ą",
"Ć",
"ć",
"Ĉ",
"ĉ",
"Ċ",
"ċ",
"Č",
"č",
"Ď",
"ď",
"Đ",
"đ",
"Ē",
"ē",
"Ė",
"ė",
"Ę",
"ę",
"Ě",
"ě",
"Ĝ",
"ĝ",
"Ğ",
"ğ",
"Ġ",
"ġ",
"Ģ",
"ģ",
"Ĥ",
"ĥ",
"Ħ",
"ħ",
"Ĩ",
"ĩ",
"Ī",
"ī",
"Į",
"į",
"İ",
"ı",
"Ĵ",
"ĵ",
"Ķ",
"ķ",
"ĸ",
"Ĺ",
"ĺ",
"Ļ",
"ļ",
"Ľ",
"ľ",
"Ł",
"ł",
"Ń",
"ń",
"Ņ",
"ņ",
"Ň",
"ň",
"Ŋ",
"ŋ",
"Ō",
"ō",
"Ő",
"ő",
"Œ",
"œ",
"Ŕ",
"ŕ",
"Ŗ",
"ŗ",
"Ř",
"ř",
"Ś",
"ś",
"Ŝ",
"ŝ",
"Ş",
"ş",
"Š",
"š",
"Ţ",
"ţ",
"Ť",
"ť",
"Ŧ",
"ŧ",
"Ũ",
"ũ",
"Ū",
"ū",
"Ŭ",
"ŭ",
"Ů",
"ů",
"Ű",
"ű",
"Ų",
"ų",
"Ÿ",
"Ź",
"ź",
"Ż",
"ż",
"Ž",
"ž",
"ƒ",
"ˇ",
"˘",
"˙",
"˛",
"˝",
"΅",
"Ά",
"Έ",
"Ή",
"Ί",
"Ό",
"Ύ",
"Ώ",
"ΐ",
"Α",
"Β",
"Γ",
"Δ",
"Ε",
"Ζ",
"Η",
"Θ",
"Ι",
"Κ",
"Λ",
"Μ",
"Ν",
"Ξ",
"Ο",
"Π",
"Ρ",
"Σ",
"Τ",
"Υ",
"Φ",
"Χ",
"Ψ",
"Ω",
"Ϊ",
"Ϋ",
"ά",
"έ",
"ή",
"ί",
"ΰ",
"α",
"β",
"γ",
"δ",
"ε",
"ζ",
"η",
"θ",
"ι",
"κ",
"λ",
"μ",
"ν",
"ξ",
"ο",
"π",
"ρ",
"ς",
"σ",
"τ",
"υ",
"φ",
"χ",
"ψ",
"ω",
"ϊ",
"ϋ",
"ό",
"ύ",
"ώ",
"Ё",
"Ђ",
"Ѓ",
"Є",
"Ѕ",
"І",
"Ї",
"Ј",
"Љ",
"Њ",
"Ћ",
"Ќ",
"Ў",
"Џ",
"А",
"Б",
"В",
"Г",
"Д",
"Е",
"Ж",
"З",
"И",
"Й",
"К",
"Л",
"М",
"Н",
"О",
"П",
"Р",
"С",
"Т",
"У",
"Ф",
"Х",
"Ц",
"Ч",
"Ш",
"Щ",
"Ъ",
"Ы",
"Ь",
"Э",
"Ю",
"Я",
"а",
"б",
"в",
"г",
"д",
"е",
"ж",
"з",
"и",
"й",
"к",
"л",
"м",
"н",
"о",
"п",
"р",
"с",
"т",
"у",
"ф",
"х",
"ц",
"ч",
"ш",
"щ",
"ъ",
"ы",
"ь",
"э",
"ю",
"я",
"ё",
"ђ",
"ѓ",
"є",
"ѕ",
"і",
"ї",
"ј",
"љ",
"њ",
"ћ",
"ќ",
"ў",
"џ",
"א",
"ב",
"ג",
"ד",
"ה",
"ו",
"ז",
"ח",
"ט",
"י",
"ך",
"כ",
"ל",
"ם",
"מ",
"ן",
"נ",
"ס",
"ע",
"ף",
"פ",
"ץ",
"צ",
"ק",
"ר",
"ש",
"ת",
"،",
"؛",
"؟",
"ء",
"آ",
"أ",
"ؤ",
"إ",
"ئ",
"ا",
"ب",
"ة",
"ت",
"ث",
"ج",
"ح",
"خ",
"د",
"ذ",
"ر",
"ز",
"س",
"ش",
"ص",
"ض",
"ط",
"ظ",
"ع",
"غ",
"ـ",
"ف",
"ق",
"ك",
"ل",
"م",
"ن",
"ه",
"و",
"ى",
"ي",
"ً",
"ٌ",
"ٍ",
"َ",
"ُ",
"ِ",
"ّ",
"ْ",
"ก",
"ข",
"ฃ",
"ค",
"ฅ",
"ฆ",
"ง",
"จ",
"ฉ",
"ช",
"ซ",
"ฌ",
"ญ",
"ฎ",
"ฏ",
"ฐ",
"ฑ",
"ฒ",
"ณ",
"ด",
"ต",
"ถ",
"ท",
"ธ",
"น",
"บ",
"ป",
"ผ",
"ฝ",
"พ",
"ฟ",
"ภ",
"ม",
"ย",
"ร",
"ฤ",
"ล",
"ฦ",
"ว",
"ศ",
"ษ",
"ส",
"ห",
"ฬ",
"อ",
"ฮ",
"ฯ",
"ะ",
"ั",
"า",
"ำ",
"ิ",
"ี",
"ึ",
"ื",
"ุ",
"ู",
"ฺ",
"฿",
"เ",
"แ",
"โ",
"ใ",
"ไ",
"ๅ",
"ๆ",
"็",
"่",
"้",
"๊",
"๋",
"์",
"ํ",
"๐",
"๑",
"๒",
"๓",
"๔",
"๕",
"๖",
"๗",
"๘",
"๙",
"ᆨ",
"ᆩ",
"ᆪ",
"ᆫ",
"ᆬ",
"ᆭ",
"ᆮ",
"ᆯ",
"ᆰ",
"ᆱ",
"ᆲ",
"ᆳ",
"ᆴ",
"ᆵ",
"ᆶ",
"ᆷ",
"ᆸ",
"ᆹ",
"ᆺ",
"ᆻ",
"ᆼ",
"ᆽ",
"ᆾ",
"ᆿ",
"ᇀ",
"ᇁ",
"ᇂ",
"ᇫ",
"ᇰ",
"ᇹ",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
" ",
"‒",
"–",
"—",
"―",
"‗",
"‘",
"’",
"‚",
"“",
"”",
"„",
"†",
"‡",
"•",
"‥",
"…",
"′",
"″",
"‸",
"‾",
"₩",
"€",
"€",
"℅",
"№",
"℗",
"℞",
"™",
"⅓",
"⅔",
"⅕",
"⅖",
"⅗",
"⅘",
"⅙",
"⅚",
"⅛",
"⅜",
"⅝",
"⅞",
"←",
"↑",
"→",
"↓",
"⇒",
"⇔",
"∂",
"∇",
"∘",
"√",
"∝",
"∞",
"∧",
"∧",
"∨",
"∨",
"∩",
"∩",
"∪",
"∪",
"∫",
"∴",
"∼",
"≃",
"≠",
"≡",
"≤",
"≥",
"⊂",
"⊂",
"⊃",
"⊃",
"⊢",
"⊣",
"⊤",
"⊥",
"⌈",
"⌊",
"⌕",
"⌠",
"⌡",
"〈",
"〉",
"⎕",
"⎛",
"⎝",
"⎞",
"⎠",
"⎡",
"⎣",
"⎤",
"⎦",
"⎨",
"⎬",
"⎷",
"⎺",
"⎻",
"⎼",
"⎽",
"␉",
"␊",
"␋",
"␌",
"␍",
"␤",
"─",
"─",
"│",
"│",
"┌",
"┌",
"┐",
"└",
"┘",
"├",
"┤",
"┬",
"┴",
"┼",
"▒",
"▪",
"▫",
"▬",
"▭",
"▮",
"▯",
"▲",
"△",
"▶",
"▷",
"▼",
"▽",
"◀",
"◁",
"◆",
"○",
"○",
"●",
"◦",
"☆",
"☎",
"☓",
"☜",
"☞",
"♀",
"♂",
"♣",
"♥",
"♦",
"♭",
"♯",
"✓",
"✗",
"✝",
"✠",
"、",
"。",
"「",
"」",
"゛",
"゜",
"ァ",
"ア",
"ィ",
"イ",
"ゥ",
"ウ",
"ェ",
"エ",
"ォ",
"オ",
"カ",
"キ",
"ク",
"ケ",
"コ",
"サ",
"シ",
"ス",
"セ",
"ソ",
"タ",
"チ",
"ッ",
"ツ",
"テ",
"ト",
"ナ",
"ニ",
"ヌ",
"ネ",
"ノ",
"ハ",
"ヒ",
"フ",
"ヘ",
"ホ",
"マ",
"ミ",
"ム",
"メ",
"モ",
"ャ",
"ヤ",
"ュ",
"ユ",
"ョ",
"ヨ",
"ラ",
"リ",
"ル",
"レ",
"ロ",
"ワ",
"ヲ",
"ン",
"・",
"ー",
"ㄱ",
"ㄲ",
"ㄳ",
"ㄴ",
"ㄵ",
"ㄶ",
"ㄷ",
"ㄸ",
"ㄹ",
"ㄺ",
"ㄻ",
"ㄼ",
"ㄽ",
"ㄾ",
"ㄿ",
"ㅀ",
"ㅁ",
"ㅂ",
"ㅃ",
"ㅄ",
"ㅅ",
"ㅆ",
"ㅇ",
"ㅈ",
"ㅉ",
"ㅊ",
"ㅋ",
"ㅌ",
"ㅍ",
"ㅎ",
"ㅏ",
"ㅐ",
"ㅑ",
"ㅒ",
"ㅓ",
"ㅔ",
"ㅕ",
"ㅖ",
"ㅗ",
"ㅘ",
"ㅙ",
"ㅚ",
"ㅛ",
"ㅜ",
"ㅝ",
"ㅞ",
"ㅟ",
"ㅠ",
"ㅡ",
"ㅢ",
"ㅣ",
"ㅭ",
"ㅱ",
"ㅸ",
"ㅿ",
"ㆁ",
"ㆄ",
"ㆆ",
"ㆍ",
"ㆎ"
};

int unicsym2char(char* towrite,int unicode)
{
    int min = 0;
    int max = sizeof(keysymcodes) / sizeof(unsigned short) - 1;
    int mid;
    
    /* binary search in table */
    while (max >= min) {
	mid = (min + max) / 2;
	if (keysymcodes[mid] < unicode)
	    min = mid + 1;
	else if (keysymcodes[mid] > unicode)
	    max = mid - 1;
	else {
	    /* found it */
	    strcat(towrite,keysymletters[mid]);
	    return 0;
	}
    }
   /* no matching Unicode value found */
    return -1;
}