module.exports = {
  extends: [
    'airbnb',
    'plugin:jsx-a11y/strict',
  ],
  env: {
    browser: true,
  },
  rules: {
    'import/no-extraneous-dependencies': 'off',
    // The SMM team doesn't write React code in .jsx files exclusively, as is suggested in the
    // Airbnb guide, so override this rule to allow .js files.
    'react/jsx-filename-extension': [
      1,
      {
        extensions: [
          '.js',
          '.jsx',
        ],
      },
    ],
  },
};
