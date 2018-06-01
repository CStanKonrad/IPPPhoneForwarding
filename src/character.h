/** @file
 * Interfejs modułu modułu do uzyskiwania informacji o znakach.
 *
 * @author Konrad Staniszewski
 * @copyright Konrad Staniszewski
 * @date 25.05.2018
 */

#ifndef MARATONFILMOWY_CHARACTER_H
#define MARATONFILMOWY_CHARACTER_H

/**
 * @brief Liczba cyfr w przyjętym systemie.
 */
#define CHARACTER_NUMBER_OF_DIGITS 12

/**
 * @brief Znak kończący ciąg znaków w stylu c.
 */
#define CHARACTER_STRING_TERMINATOR '\0'

/**
 * @brief Znak uniksowej nowej lini.
 */
#define CHARACTER_UNIX_NEW_LINE '\n'

/**
 * @brief Znak powrotu karetki.
 */
#define CHARACTER_CARRIAGE_RETURN '\r'

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli @p characterCode reprezentuje:
 *         spacje, tabulator.
 */
int characterIsBlank(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return niezerową wartość jeżeli characterCode reprezentuje:
 *         CHARACTER_UNIX_NEW_LINE.
 */
int characterIsUnixNewLine(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * CHARACTER_CARRIAGE_RETURN.
 */
int characterIsCarriageReturn(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * characterIsUnixNewLine, characterIsCarriageReturn.
 */
int characterIsNewLine(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * EOF.
 */
int characterIsEOF(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * characterIsNewLine, characterIsEOF.
 */
int characterIsTerminator(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * characterIsUnixNewLine, characterIsEOF.
 */
int characterIsUnixTerminator(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * spacje, \ f, \ r, \ t, \ v.
 */
int characterIsWhite(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * drukowalny znak poza spacją.
 */
int characterIsGraph(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * cyfrę ze zbioru {0, ..., 9, :, ;}.
 */
int characterIsDigit(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * literę.
 */
int characterIsLetter(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * minus.
 */
int characterIsMinus(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * plus.
 */
int characterIsPlus(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * zero.
 */
int characterIsZero(int characterCode);

/**
 * @param[in] characterCode - kod znaku.
 * @return Niezerowa wartość jeżeli characterCode reprezentuje:
 * spację.
 */
int characterIsSpaceBar(int characterCode);

#endif //MARATONFILMOWY_CHARACTERS_H
