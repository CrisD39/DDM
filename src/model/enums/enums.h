#ifndef ENUMS_H
#define ENUMS_H

enum class Type { Air, Surface, Subsurface, RP, ESM };

enum class Identity { Pending, PossHostile, PossFriend, ConfHostile, ConfFriend, EvalUnknown, Heli };

enum class TrackMode { FC1, FC2, FC3, FC4, FC5, Auto, TentativeAuto, AutomaticLost, RAM, DR, Lost, Blank };

#endif // ENUMS_H
