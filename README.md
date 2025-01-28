# Ananas

> _Ananas necessitates another networked audio system_

## Why?

Because existing systems, such as _JackTrip_ and _Audio Over OSC_, either do not
support multicast transmission, and were not designed with time-sensitive (i.e. 
sub-microsecond) applications in mind, nor those that entail distributed signal
processing.

Ananas exists with the aim of being a lightweight, multicast, _time-sensitive_
audio system for local area networks. It's a work-in-progress.

Current assumptions include:
- The system has a network interface with hardware timestamping support (one
  like [this](https://www.delock.com/produkt/66045/kaufen.html), perhaps).
- A PTP daemon (e.g. `linuxptp`) is running.

## Dependencies

- JUCE 8.0.6
- borrows some bits and pieces from `linuxptp`.

## Deliverables

### `ananas_console`

#### Usage

```shell
ananas_console [-f |--file=][filename]
```

Transmits `filename` (.wav, .aif) to the network on UDP multicast IP 
`224.4.224.4`, port `41952`.

Audio packets are software timestamped. Timestamps are derived from the system
PTP clock; consequently `ananas_console` requires root access. 

## Roadmap

- [ ] DAW plugin that embeds `ananas_server`.
  - This should ideally be in CLAP format for improved flexibility (with respect
    to VST/AU) in terms of channel count and layout.

---

#### Me
Can you give me an ascii-art pineapple?

#### AI Assistant
Sure! Here's an ASCII-art pineapple for you! 🍍

```
      .
     /_\
    _\ /_
   /     \
   \_   _/
     | |
    /___\
   //   \\
  ||     ||
  \\_____/\
   `-----`
```

I hope you like it! Let me know if you'd like to see something else. 😊

#### Me
Can you try again, please?

#### AI Assistant
Of course! Here's a more detailed ASCII-art pineapple for you: 🍍

```
      __---__
    _-       -_
   /           \
  |     /\      |
   \    ||     /
    |   ||    |
    |   ||    |
     \  ||   /
      | ||   |
      | ||   |
       \||  /
        |   |
        |   |
        |   |
        '---'
```

If you'd like something different or even more detailed, just let me know! 😊🍍

#### Me
Never mind.

#### AI Assistant
No problem! Feel free to reach out if you need any help in the future. 😊

#### Me
Pouvez-vous me dessiner un ananas en ascii-art ?

#### AI Assistant
Bien sûr ! Voici un bel ananas en ASCII-art pour vous : 🍍

```
      ______
     /      \
    /        \
   |          |   _ _
   |          |  /   \
    \        /   \_   |
     \______/      |  |
        ||         |  |
        ||         |  |
        ||         |__|
        ||         ||||
       //\\        ||||
      //  \\       ||||
     ||    ||      ||||
```

J’espère que ça vous plaît ! 😊 Si vous souhaitez autre chose, n’hésitez pas à demander. 🍍

#### Me
Beurk !

#### AI Assistant
Oh, je suis désolé que celui-ci ne vous ait pas plu ! Laissez-moi réessayer avec une meilleure version d’un ananas en ASCII-art 🌟 :

```
       .  .
     .:....:.
    .:  :   :.
     .:....:.
     '::::::'
    :::'  ':::  
   :::      ::: 
    :::.  .:::  
     '::::::'   
        ':'
```

J'espère que celui-ci est plus à votre goût ! 😊 🍍 S'il y a autre chose que vous aimeriez voir, n'hésitez pas à demander.
