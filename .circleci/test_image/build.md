# Instructions for building the CircleCI docker image for testing

- install docker desktop
- cd to this directory
- docker build -t streampunkmedia/testbeam:x-y.z .
- run container locally to check build
- push to Docker Hub
- update config.yml to pull new version tag
- push to git to trigger new build and test

(x: NodeAPI base version, y.z: FFmpeg build number)

See https://circleci.com/developer/images/image/cimg/node for CircleCI docker image tags
