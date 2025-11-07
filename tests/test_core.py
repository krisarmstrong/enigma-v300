from enigma_v300.core import EnigmaC, Enigma2C


def test_enigmac_roundtrip():
    enigmac = EnigmaC()
    key = "0003333016"
    encrypted = enigmac.encrypt(key)
    assert enigmac.decrypt(encrypted)[:10] == key


def test_enigmac_known_option():
    enigmac = EnigmaC()
    serial = "0003333016"
    option = 4
    reversed_key = (serial + str(option) + "0")[::-1]
    assert enigmac.encrypt(reversed_key) == "5dabade112dd"


def test_enigma2_sample():
    enigma2 = Enigma2C()
    input_key = "00" + "6963" + "0000607" + "007"
    assert enigma2.encrypt(input_key) == "6406257948597747"
    decrypted = enigma2.decrypt("6406257948597747")
    assert decrypted.endswith("007")
    assert decrypted[2:6] == "6963"
    assert decrypted[6:13] == "0000607"
