<div id="iathena-logo" align="center">
    <br />
    <img src="./docs/logo_iathena_big.png" alt="Iathena Logo" width="200"/>
    <h1>IATHENA</h1>
    <h3>Free Open Source UCI Chess Engine</h3>
</div>

<div id="badges" align="center">

[![gitHub release](https://img.shields.io/github/release/tschmoderer/iathena.svg)](https://github.com/tschmoderer/iathena/releases)
&nbsp; 
[![Build Status](https://travis-ci.org/tschmoderer/iathena.png?branch=master)](https://travis-ci.org/tschmoderer/iathena)
&nbsp;
[![Build status](https://ci.appveyor.com/api/projects/status/etev4kfqsf9l4s2s/branch/master?svg=true)](https://ci.appveyor.com/project/tschmoderer/iathena/branch/master)
&nbsp; 
[![license](https://img.shields.io/github/license/tschmoderer/iathena.svg?color=blue)](https://github.com/tschmoderer/iathena/blob/master/LICENSE)

</div>

IATHENA is a chess engine built in C++ using the UCI protocol. IATHENA is deployed on [Lichess](https://lichess.org/@/iathena). If online, please feel free to drop a challenge.

## Getting Started

You are welcome to use IATHENA for your own personnal projects, to clone the repo : 

```
git clone https://github.com/tschmoderer/iathena.git
```

IATHENA code is located in CPP_IATHENA folder. Run make to compile your own version of IATHENA. 

```
cd src
make
```

If you want to deploy IATHENA on [Lichess](https://lichess.org/) please check the Lichess [API](https://lichess.org/api) documentation. And use [lichess-bot](https://github.com/careless25/lichess-bot) wrapper to deploy. 

You can deploy IATHENA in any chess server you like, please let the author know you have deployed IATHENA, see the *Contact* section below.  

### Documentation

The project is fully documented with [doxygen](http://www.doxygen.nl/) and is available at [https://tschmoderer.github.io/iathena/](https://tschmoderer.github.io/iathena/).

### Ratings

<p align="center">
  <img src="https://github.com/tschmoderer/iathena/blob/master/releases/elo/elo_ratings.png" alt="Evolution of IATHENA rating"/>
</p>

## Authors

* **Timothée Schmoderer** - *Initial work* - [https://github.com/tschmoderer](https://github.com/tschmoderer)

See also the list of [contributors](https://github.com/your/project/contributors) who participated in this project.

## License

This project is licensed under the GNU General Public License - see the [LICENSE](LICENSE) file for details. 

## Acknowledgments

* IATHENA first version is inspired by the tutorial of [Bluefever Software](https://www.youtube.com/playlist?list=PLZ1QII7yudbc-Ky058TEaOstZHVbT-2hg) on YouTube.
* The authors would like to thank all the [Lichess](https://github.com/ornicar/lila) developpement team for their inspiring work. We are glad to deploy our engine on this webside.
* IATHENA plays on Lichess via the [lichess-bot](https://github.com/careless25/lichess-bot) framework
* Testing framework based on [cutechess-cli](https://github.com/cutechess/cutechess/releases)
* ELO computing based on [ordo](https://github.com/michiguel/Ordo)
* Skeleton of this README is based on template by [PurpleBooth](https://gist.github.com/PurpleBooth/109311bb0361f32d87a2).
* Great inspirations from [Stockfish](https://stockfishchess.org/), [Shallow Blue](https://github.com/GunshipPenguin/shallow-blue) and [ToppleChess](https://github.com/konsolas/ToppleChess)

## Contact

<div id="badges" align="center">

[![Twitter URL](https://img.shields.io/twitter/url/https/iathenaChess.svg?style=social)](https://twitter.com/iathenaChess)
&nbsp; &nbsp; 
[![Mail](https://img.shields.io/badge/Mail-iathena%40mailo.com-blue.svg?style=flat&logo=MinuteMailer)](mailto:iathena@mailo.com)

</div>


