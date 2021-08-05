# BimmerLED's BLE Protocol

The protocol is purposefully made easy to understand, human-readable, yet trying to leave the smallest fingerprint as possible.

Do not exceed an json length of 182 bytes. This is the maximum for iPhone.

If you want to send more than that, you need to sent multiple packets. THIS IS NOT SUPPORTED YET.

## Send a packet

- PREFER operant's payload to be an `object`, otherwise literal if only one value is passed
- packet MUST an array with 1-3 values with the following format:

```json
// operation-id     operant's payload     meta
   [int,            any?,                  object?]
```

[@see Meta](#meta)
[@see Operation ID](#operation-id)

**@example**

```json
[0, 1627901614]
[0, 1627901614, {"dry": true}]
```

## Receive a packet

- An packet MUST return an meta object, if no values exist, an empty object is returned
- The packet MUST be an array with 3 values in the following format:

```json
// operation-id     operant's response    meta
   [int,            any,                  object]
```

[@see Meta](#meta)
[@see Operation ID](#operation-id)

**@example**

```json
[0, {"got": 1627901614, "millis": 53145}, {}]
[0, null, { "__NOT_IMPLEMENTED__": true }]
```

## Meta

- An response will always have an meta object - even if it's empty
- The meta object MUST inherited request's meta
- Operator function MAY modify the meta object
- Keys whose start and end with `__` are internal

**example**

```json
sent: [0, 1627901614]
receive: [0, {"got": 1627901614, "millis": 53145}, {}]

sent: [0, 1627901614, {"dry": true}]
receive: [0, {"got": 1627901614, "millis": 53145}, {"dry": true}]
receive: [0, {"got": 1627901614, "millis": 53145}, {"dry": true, "reverted": true}]
```

## Operation ID

- The operation id MUST be greater than 0
- The operation id CAN NOT exceed 50, unless it's a [compatibility id](#compatibility)
- Operation id 0 is reserved for [Ping](#ping) **and cannot change**

### Compatibility

Once an operation id has been shipped, it's payload signature and id cannot be changed anymore.
However, there are up to 5 breaking changes allowed.

If an breaking change occours, a new operation id must be generated using the formula `x + (50 * c)`.
Where `x` is the operation id which is going to be deprecated and `c` the [compatabillity counter](#compatabillity-counter).

An deprecated operation id MUST always respond with `{"__new__": <new operation id>}` within the meta object.

#### Compatabillity Counter

- You are allowed to use up to 5 breaking changes.
- If the counter exceeds 5, you MUST reset the counter.
- You MUST ensure compatabillity for at least 1 version.
- You MAY be able to reset the compatabillity counter to 0 if a reasonable time for migration passed.

## Operations

### Ping

A ping will test the connection between the app and the controller.
It will return a payload with whatever you provided in the request's payload to verify the connection.

#### Request

| Operation ID | Payload                      | Meta |
| ------------ | ---------------------------- | ---- |
| 0            | _usually the unix timestamp_ |      |

#### Response

| Operation ID | Payload                                          | Meta |
| ------------ | ------------------------------------------------ | ---- |
| 0            | `{"got": <request payload>, "millis": millis()}` |      |

### GetAbout

Responds with controller and hardware informations.

#### Request

| Operation ID | Payload | Meta |
| ------------ | ------- | ---- |
| 1            |         |      |

#### Response

| Operation ID | Payload                                                             | Meta |
| ------------ | ------------------------------------------------------------------- | ---- |
| 1            | `{ "skd": "string", "rev": "string", "c": "string", "v": "string"}` |      |

- `skd` The SKD version of the ESP
- `rev` The Chip Revision of the ESP
- `c` Internal name of the BimmerLED controller
- `v` version of the BimmerLED controller

### GetState

Responds with the current overall state of the controller's functions.

#### Request

| Operation ID | Payload | Meta |
| ------------ | ------- | ---- |
| 2            |         |      |

#### Response

| Operation ID | Payload                                                             | Meta |
| ------------ | ------------------------------------------------------------------- | ---- |
| 2            | `{ "skd": "string", "rev": "string", "c": "string", "v": "string"}` |      |

- `skd` The SKD version of the ESP
- `rev` The Chip Revision of the ESP
- `c` Internal name of the BimmerLED controller
- `v` version of the BimmerLED controller

# This is a living document.
